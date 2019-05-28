/*
 * fs.c
 *
 *  Created on: 27 May 2019
 *      Author: m.bilge
 */

/*
#include <FreeRTOS.h>

#include <esp_system.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "fs.c"

int fs_open_custom(struct fs_file *file, const char *name);
void fs_close_custom(struct fs_file *file);

err_t fs_open(struct fs_file *file, const char *name)
{
  const struct fsdata_file *f;

  if ((file == NULL) || (name == NULL)) {
     return ERR_ARG;
  }

  if (fs_open_custom(file, name)) {
    file->is_custom_file = 1;
    return ERR_OK;
  }
  file->is_custom_file = 0;

  for (f = FS_ROOT; f != NULL; f = f->next) {
    if (!strcmp(name, (char *)f->name)) {
      file->data = (const char *)f->data;
      file->len = f->len;
      file->index = f->len;
      file->pextension = NULL;
      file->http_header_included = f->http_header_included;
      file->chksum_count = f->chksum_count;
      file->chksum = f->chksum;

#if LWIP_HTTPD_FILE_STATE
      file->state = fs_state_init(file, name);
      return ERR_OK;
    }
  }
  // file not found
  return ERR_VAL;
}
*/
