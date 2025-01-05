#include "facilities.h"

int facility_load(char* path);

int facility_save();

int facility_save_as(char* path);

int facility_close();

int facility_fetch(char* identifier);

int facility_gen(char* identifier, unsigned short length);

int facility_remove(char* identifier);

int facility_memorize(char* identifier);

int facility_memorize_settings_get(char* setting_name);

int facility_memorize_settings_set(char* setting_name, char* setting_value);

int facility_transfer_send(int mode);

int facility_transfer_receive(int mode);
