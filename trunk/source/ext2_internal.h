#ifndef EXT2_INTERNAL_H_
#define EXT2_INTERNAL_H_

#include <gccore.h>
#include <ogc/disc_io.h>
#include <sys/iosupport.h>
#include "ext2fs.h"
#include "ext2_fs.h"
#include "mem_allocate.h"

#define ext2_log_trace(...)

typedef struct _ext2_inode_t
{
    struct ext2_inode ni;
    ext2_ino_t ino;
    bool dirty;
} ext2_inode_t;

/**
 * ext2_vd - EXT2 volume descriptor
 */
typedef struct _ext2_vd
{
	io_channel io;                          /* NTFS device handle */
    ext2_filsys fs;                         /* NTFS volume handle */
    mutex_t lock;                           /* Volume lock mutex */
//    u16 uid;                                /* User id for entry creation */
//    u16 gid;                                /* Group id for entry creation */
//    u16 fmask;                              /* Unix style permission mask for file creation */
//    u16 dmask;                              /* Unix style permission mask for directory creation */
//    bool showHiddenFiles;                   /* If true, show hidden files when enumerating directories */
//    bool showSystemFiles;                   /* If true, show system files when enumerating directories */
    ext2_inode_t *cwd_ni;                   /* Current directory */
    struct _ext2_dir_state *firstOpenDir;   /* The start of a FILO linked list of currently opened directories */
    struct _ext2_file_state *firstOpenFile; /* The start of a FILO linked list of currently opened files */
    u16 openDirCount;                       /* The total number of directories currently open in this volume */
    u16 openFileCount;                      /* The total number of files currently open in this volume */
    ext2_ino_t root;                        /* Root node */
} ext2_vd;

typedef enum {
	EXT2_UPDATE_ATIME = 0x01,
	EXT2_UPDATE_MTIME = 0x02,
	EXT2_UPDATE_CTIME = 0x04,
	EXT2_UPDATE_AMTIME = EXT2_UPDATE_ATIME | EXT2_UPDATE_MTIME,
	EXT2_UPDATE_ACTIME = EXT2_UPDATE_ATIME | EXT2_UPDATE_CTIME,
	EXT2_UPDATE_MCTIME = EXT2_UPDATE_MTIME | EXT2_UPDATE_CTIME,
	EXT2_UPDATE_AMCTIME = EXT2_UPDATE_ATIME | EXT2_UPDATE_MTIME | EXT2_UPDATE_CTIME,
} ext2_time_update_flags;

/* Lock volume */
static inline void ext2Lock (ext2_vd *vd)
{
    LWP_MutexLock(vd->lock);
}

/* Unlock volume */
static inline void ext2Unlock (ext2_vd *vd)
{
    LWP_MutexUnlock(vd->lock);
}

const char *ext2RealPath (const char *path);
int ext2InitVolume (ext2_vd *vd);
void ext2DeinitVolume (ext2_vd *vd);
ext2_vd *ext2GetVolume (const char *path);

int ext2AddDevice (const char *name, void *deviceData);
void ext2RemoveDevice (const char *path);
const devoptab_t *ext2GetDevice (const char *path);

ext2_inode_t *ext2OpenEntry (ext2_vd *vd, const char *path);
void ext2CloseEntry (ext2_vd *vd, ext2_inode_t * ni);
int ext2Stat (ext2_vd *vd, ext2_inode_t * ni, struct stat *st);
int ext2Sync (ext2_vd *vd, ext2_inode_t * ni);

ext2_inode_t *ext2Create (ext2_vd *vd, const char *path, mode_t type, const char *target);
int ext2Link (ext2_vd *vd, const char *old_path, const char *new_path);
int ext2Unlink (ext2_vd *vd, const char *path);

void ext2UpdateTimes(ext2_vd *vd, ext2_inode_t *ni, ext2_time_update_flags mask);

#endif
