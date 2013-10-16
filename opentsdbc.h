#ifndef __include_opentsdbc_h__
#define __include_opentsdbc_h__
void sendCollectedData(int now, char * metric, char * data, char * tag);
int sendCollectedData2(int now, char * metric, char * data, char * tag, char * old_data, int lastupdate);
#endif /* #ifndef __include_opentsdbc_h__ */
