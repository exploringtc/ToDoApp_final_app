/*
 * PeachOS 32-Bit Kernel project
 */

#include "todo.h"
#include "task/task.h"
#include "string/string.h"
#include "memory/memory.h"
#include "disk/disk.h"
#include "status.h"
#include "kernel.h"

#define TODO_MAX_TASKS 24
#define TODO_DESC_MAX 64
#define TODO_FILENAME_MAX 32
#define TODO_KEY_MAX 64

#define TODO_SAVE_SLOTS 8
#define TODO_SLOT_SECTORS 8
#define TODO_SLOT_BYTES (TODO_SLOT_SECTORS * 512)
#define TODO_CATALOG_LBA 32000
#define TODO_DATA_BASE_LBA (TODO_CATALOG_LBA + 1)

struct todo_task
{
    int id;
    int active;
    int complete;
    char description[TODO_DESC_MAX];
};

struct todo_catalog
{
    char magic[8];
    int version;
    unsigned char used[TODO_SAVE_SLOTS];
    unsigned int lengths[TODO_SAVE_SLOTS];
    char names[TODO_SAVE_SLOTS][TODO_FILENAME_MAX];
};

struct todo_serial_header
{
    char magic[8];
    int next_id;
    int task_count;
};

struct todo_serial_entry
{
    int id;
    int complete;
    char description[TODO_DESC_MAX];
};

static struct todo_task todo_tasks[TODO_MAX_TASKS];
static int todo_next_id = 1;

static void todo_print_int(int value)
{
    char out[16];
    int pos = 15;
    int negative = 0;
    out[15] = 0;

    if (value == 0)
    {
        print("0");
        return;
    }

    if (value < 0)
    {
        negative = 1;
        value = -value;
    }

    while(value > 0 && pos > 0)
    {
        int digit = value % 10;
        out[--pos] = '0' + digit;
        value /= 10;
    }

    if (negative && pos > 0)
    {
        out[--pos] = '-';
    }

    print(&out[pos]);
}

static int todo_find_free_slot()
{
    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (!todo_tasks[i].active)
        {
            return i;
        }
    }

    return -1;
}

static int todo_find_task_by_id(int id)
{
    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (todo_tasks[i].active && todo_tasks[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

static int todo_task_count()
{
    int count = 0;
    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (todo_tasks[i].active)
        {
            count++;
        }
    }

    return count;
}

static void todo_xor_crypt(char* data, int len, const char* key, int key_len)
{
    for (int i = 0; i < len; i++)
    {
        data[i] = data[i] ^ key[i % key_len];
    }
}

static void todo_catalog_default(struct todo_catalog* catalog)
{
    memset(catalog, 0, sizeof(struct todo_catalog));
    strncpy(catalog->magic, "TDOCAT1", sizeof(catalog->magic));
    catalog->version = 1;
}

static int todo_catalog_load(struct todo_catalog* catalog)
{
    struct disk* disk = disk_get(0);
    if (!disk)
    {
        return -EIO;
    }

    char sector[512];
    int res = disk_read_block(disk, TODO_CATALOG_LBA, 1, sector);
    if (res < 0)
    {
        return res;
    }

    memcpy(catalog, sector, sizeof(struct todo_catalog));
    if (strncmp(catalog->magic, "TDOCAT1", 7) != 0 || catalog->version != 1)
    {
        todo_catalog_default(catalog);
    }

    return 0;
}

static int todo_catalog_save(struct todo_catalog* catalog)
{
    struct disk* disk = disk_get(0);
    if (!disk)
    {
        return -EIO;
    }

    char sector[512];
    memset(sector, 0, sizeof(sector));
    memcpy(sector, catalog, sizeof(struct todo_catalog));
    return disk_write_block(disk, TODO_CATALOG_LBA, 1, sector);
}

static int todo_save_slot_index(struct todo_catalog* catalog, const char* filename)
{
    for (int i = 0; i < TODO_SAVE_SLOTS; i++)
    {
        if (catalog->used[i] && strncmp(catalog->names[i], filename, TODO_FILENAME_MAX) == 0)
        {
            return i;
        }
    }

    for (int i = 0; i < TODO_SAVE_SLOTS; i++)
    {
        if (!catalog->used[i])
        {
            return i;
        }
    }

    return -1;
}

static int todo_find_slot_index(struct todo_catalog* catalog, const char* filename)
{
    for (int i = 0; i < TODO_SAVE_SLOTS; i++)
    {
        if (catalog->used[i] && strncmp(catalog->names[i], filename, TODO_FILENAME_MAX) == 0)
        {
            return i;
        }
    }

    return -1;
}

static int todo_save_to_disk(const char* filename, const char* key)
{
    int key_len = strnlen(key, TODO_KEY_MAX);
    if (key_len <= 0)
    {
        return -EINVARG;
    }

    struct todo_catalog catalog;
    int res = todo_catalog_load(&catalog);
    if (res < 0)
    {
        return res;
    }

    int slot = todo_save_slot_index(&catalog, filename);
    if (slot < 0)
    {
        return -ENOMEM;
    }

    char plain[TODO_SLOT_BYTES];
    char enc[TODO_SLOT_BYTES];
    memset(plain, 0, sizeof(plain));
    memset(enc, 0, sizeof(enc));

    struct todo_serial_header header;
    memset(&header, 0, sizeof(header));
    strncpy(header.magic, "TDODAT1", sizeof(header.magic));
    header.next_id = todo_next_id;
    header.task_count = todo_task_count();

    int offset = 0;
    memcpy(plain + offset, &header, sizeof(header));
    offset += sizeof(header);

    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (!todo_tasks[i].active)
        {
            continue;
        }

        struct todo_serial_entry entry;
        memset(&entry, 0, sizeof(entry));
        entry.id = todo_tasks[i].id;
        entry.complete = todo_tasks[i].complete;
        strncpy(entry.description, todo_tasks[i].description, sizeof(entry.description));

        if (offset + (int) sizeof(entry) > TODO_SLOT_BYTES)
        {
            return -ENOMEM;
        }

        memcpy(plain + offset, &entry, sizeof(entry));
        offset += sizeof(entry);
    }

    memcpy(enc, plain, sizeof(plain));
    todo_xor_crypt(enc, offset, key, key_len);

    struct disk* disk = disk_get(0);
    if (!disk)
    {
        return -EIO;
    }

    unsigned int lba = TODO_DATA_BASE_LBA + (slot * TODO_SLOT_SECTORS);
    for (int i = 0; i < TODO_SLOT_SECTORS; i++)
    {
        res = disk_write_block(disk, lba + i, 1, enc + (i * 512));
        if (res < 0)
        {
            return res;
        }
    }

    catalog.used[slot] = 1;
    catalog.lengths[slot] = offset;
    strncpy(catalog.names[slot], filename, TODO_FILENAME_MAX);

    return todo_catalog_save(&catalog);
}

static int todo_load_from_disk(const char* filename, const char* key)
{
    int key_len = strnlen(key, TODO_KEY_MAX);
    if (key_len <= 0)
    {
        return -EINVARG;
    }

    struct todo_catalog catalog;
    int res = todo_catalog_load(&catalog);
    if (res < 0)
    {
        return res;
    }

    int slot = todo_find_slot_index(&catalog, filename);
    if (slot < 0)
    {
        return -EINVARG;
    }

    if (catalog.lengths[slot] <= 0 || catalog.lengths[slot] > TODO_SLOT_BYTES)
    {
        return -EINVARG;
    }

    char enc[TODO_SLOT_BYTES];
    char plain[TODO_SLOT_BYTES];
    memset(enc, 0, sizeof(enc));
    memset(plain, 0, sizeof(plain));

    struct disk* disk = disk_get(0);
    if (!disk)
    {
        return -EIO;
    }

    unsigned int lba = TODO_DATA_BASE_LBA + (slot * TODO_SLOT_SECTORS);
    for (int i = 0; i < TODO_SLOT_SECTORS; i++)
    {
        res = disk_read_block(disk, lba + i, 1, enc + (i * 512));
        if (res < 0)
        {
            return res;
        }
    }

    memcpy(plain, enc, sizeof(enc));
    todo_xor_crypt(plain, catalog.lengths[slot], key, key_len);

    struct todo_serial_header header;
    memset(&header, 0, sizeof(header));
    if ((int) sizeof(header) > (int) catalog.lengths[slot])
    {
        return -EINVARG;
    }

    memcpy(&header, plain, sizeof(header));
    if (strncmp(header.magic, "TDODAT1", 7) != 0)
    {
        return -EINVARG;
    }

    memset(todo_tasks, 0, sizeof(todo_tasks));
    todo_next_id = header.next_id;

    int offset = sizeof(header);
    for (int i = 0; i < header.task_count && i < TODO_MAX_TASKS; i++)
    {
        if (offset + (int) sizeof(struct todo_serial_entry) > (int) catalog.lengths[slot])
        {
            break;
        }

        struct todo_serial_entry entry;
        memcpy(&entry, plain + offset, sizeof(entry));
        offset += sizeof(entry);

        todo_tasks[i].id = entry.id;
        todo_tasks[i].active = 1;
        todo_tasks[i].complete = entry.complete;
        strncpy(todo_tasks[i].description, entry.description, sizeof(todo_tasks[i].description));
    }

    if (todo_next_id <= 0)
    {
        todo_next_id = 1;
    }

    return 0;
}

void* isr80h_command10_todo_add(struct interrupt_frame* frame)
{
    void* task_ptr = task_get_stack_item(task_current(), 0);
    char task_desc[TODO_DESC_MAX];
    int res = copy_string_from_task(task_current(), task_ptr, task_desc, sizeof(task_desc));
    if (res < 0)
    {
        return ERROR(-EINVARG);
    }

    if (strnlen(task_desc, sizeof(task_desc)) <= 0)
    {
        return ERROR(-EINVARG);
    }

    int slot = todo_find_free_slot();
    if (slot < 0)
    {
        return ERROR(-ENOMEM);
    }

    todo_tasks[slot].id = todo_next_id++;
    todo_tasks[slot].active = 1;
    todo_tasks[slot].complete = 0;
    strncpy(todo_tasks[slot].description, task_desc, sizeof(todo_tasks[slot].description));

    return (void*) todo_tasks[slot].id;
}

void* isr80h_command11_todo_list(struct interrupt_frame* frame)
{
    (void) frame;

    int count = todo_task_count();
    if (count == 0)
    {
        print("No tasks available\n");
        return 0;
    }

    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (!todo_tasks[i].active)
        {
            continue;
        }

        print("[");
        todo_print_int(todo_tasks[i].id);
        print("] ");
        print(todo_tasks[i].description);
        print("\n");
    }

    return 0;
}

void* isr80h_command12_todo_remove(struct interrupt_frame* frame)
{
    (void) frame;

    int id = (int) task_get_stack_item(task_current(), 0);
    int slot = todo_find_task_by_id(id);
    if (slot < 0)
    {
        return ERROR(-EINVARG);
    }

    memset(&todo_tasks[slot], 0, sizeof(struct todo_task));
    return 0;
}

void* isr80h_command13_todo_save(struct interrupt_frame* frame)
{
    (void) frame;

    void* filename_ptr = task_get_stack_item(task_current(), 0);
    void* key_ptr = task_get_stack_item(task_current(), 1);

    char filename[TODO_FILENAME_MAX];
    char key[TODO_KEY_MAX];

    int res = copy_string_from_task(task_current(), filename_ptr, filename, sizeof(filename));
    if (res < 0)
    {
        return ERROR(-EINVARG);
    }

    res = copy_string_from_task(task_current(), key_ptr, key, sizeof(key));
    if (res < 0)
    {
        return ERROR(-EINVARG);
    }

    if (strnlen(filename, sizeof(filename)) <= 0)
    {
        return ERROR(-EINVARG);
    }

    res = todo_save_to_disk(filename, key);
    if (res < 0)
    {
        return ERROR(res);
    }

    return 0;
}

void* isr80h_command14_todo_load(struct interrupt_frame* frame)
{
    (void) frame;

    void* filename_ptr = task_get_stack_item(task_current(), 0);
    void* key_ptr = task_get_stack_item(task_current(), 1);

    char filename[TODO_FILENAME_MAX];
    char key[TODO_KEY_MAX];

    int res = copy_string_from_task(task_current(), filename_ptr, filename, sizeof(filename));
    if (res < 0)
    {
        return ERROR(-EINVARG);
    }

    res = copy_string_from_task(task_current(), key_ptr, key, sizeof(key));
    if (res < 0)
    {
        return ERROR(-EINVARG);
    }

    if (strnlen(filename, sizeof(filename)) <= 0)
    {
        return ERROR(-EINVARG);
    }

    res = todo_load_from_disk(filename, key);
    if (res < 0)
    {
        return ERROR(res);
    }

    return 0;
}
