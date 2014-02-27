#ifndef __ULTIMARC_H_
#define __ULTIMARC_H_


/* Unix */
#include <json.h>
#include <stdbool.h>

#define READ_CHUNK 1024

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ultimarc_type {
  ultimarc_none,
  ultimarc_ipac,
  ultimarc_ultistik,
  ultimarc_pacdrive
} ultimarc_type;

/*
 * load_json
 * Parameter: path to a json file
 * Returns: true successfully parsed the data, false otherwise
 */
extern enum ultimarc_type determine_device (json_object* jobj);

 extern void foo (void);

 void initialize ();


 extern int validate_json (char *json);

 #ifdef __cplusplus
}
#endif

#endif  // Ultimarc h
