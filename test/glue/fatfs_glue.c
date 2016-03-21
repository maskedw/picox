#include <picox/core/xcore.h>
#include <diskio.h>


typedef struct
{
    DSTATUS status;
    size_t  sz_sector;
    size_t  n_sectors;
    FILE*   diskfp;
} FakeDiskIO;


#define SZ_SECTOR (512)
#define SZ_DISK   (1024 * 1024 * 100)
#define N_SECTORS (SZ_DISK / SZ_SECTOR)

static FakeDiskIO fakeio = {STA_NOINIT, 512, (1024 * 1024 * 100) / 512, NULL};
static FakeDiskIO* const priv = &fakeio;


/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */

DWORD get_fattime (void)
{
    /* Pack date and time into a DWORD variable */
    return    ((DWORD)(0 << 25)
            | ((DWORD)1 << 21)
            | ((DWORD)1 << 16)
            | (WORD)1 << 11)
            | (WORD)(0 << 5)
            | (WORD)(0 >> 1);
}


DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive nmuber */
)
{
    X_UNUSED(pdrv);
    if (!priv->diskfp)
    {
        FILE* fp = fopen("./fatdisk.img", "w+b");
        X_ASSERT(fp);
        priv->diskfp = fp;
    }
    priv->status = 0;

    return 0;
}

void disk_delete(void)
{
    if (priv->diskfp)
    {
        fclose(priv->diskfp);
        remove("./fatdisk.img");
        priv->diskfp = NULL;
    }
}


DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber (0) */
)
{
    X_UNUSED(pdrv);

    return priv->status;
}


DRESULT disk_read (
    BYTE pdrv,          /* Physical drive nmuber (0) */
    BYTE *buff,         /* Pointer to the data buffer to store read data */
    DWORD sector,       /* Start sector number (LBA) */
    UINT count          /* Number of sectors to read */
)
{
    X_UNUSED(pdrv);
    int ret;
    ret = fseek(priv->diskfp, priv->sz_sector * sector, SEEK_SET);
    X_ASSERT(ret != -1);

    size_t readret = fread(buff, 1, priv->sz_sector * count, priv->diskfp);
    X_ASSERT(readret == priv->sz_sector * count);
    // printf("diskread {sec:%u} {cnt:%u}\n", sector, count);

    return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber (0) */
    const BYTE *buff,   /* Pointer to the data to be written */
    DWORD sector,       /* Start sector number (LBA) */
    UINT count          /* Number of sectors to write */
)
{
    X_UNUSED(pdrv);

    int ret;
    ret = fseek(priv->diskfp, priv->sz_sector * sector, SEEK_SET);
    X_ASSERT(ret != -1);

    size_t writeret = fwrite(buff, 1, priv->sz_sector * count, priv->diskfp);
    X_ASSERT(writeret == priv->sz_sector * count);
    // printf("diskwrite {sec:%u} {cnt:%u}\n", sector, count);

    return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0) */
    BYTE ctrl,      /* Control code */
    void *buff      /* Buffer to send/receive data block */
)
{
    X_UNUSED(pdrv);
    DRESULT res = RES_PARERR;


    if (priv->status & STA_NOINIT)
        return RES_NOTRDY;

    switch (ctrl) {
    case CTRL_SYNC:         /* Nothing to do */
        fflush(priv->diskfp);
        res = RES_OK;
        break;

    case GET_SECTOR_COUNT:  /* Get number of sectors on the drive */
        *(DWORD*)buff = priv->n_sectors;
        res = RES_OK;
        break;

    case GET_SECTOR_SIZE:   /* Get size of sector to read/write */
        *(WORD*)buff = priv->sz_sector;
        res = RES_OK;
        break;

    case GET_BLOCK_SIZE:    /* Get internal block size in unit of sector */
        *(DWORD*)buff = 1;
        res = RES_OK;
        break;

    default:
        break;

    }
    return res;
}
