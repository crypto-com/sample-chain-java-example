

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include "../../../chain-core.h"
#include "../../../chain.h"

//ret 1: continue, 0: stop
int32_t progress(uint64_t current, uint64_t start, uint64_t end, const void*  user_data)
{
    double gap= 0;
    double rate=0;
    if (end>start) {
        gap= end- start;
        rate= (current-start)/ gap*100.0;
    }
    char* user= (char*)user_data;
    printf("%8.2lf  progress current=%lu  start= %lu ~ end=%lu   user= %s\n",rate, current,start, end, user);
    return 1;
}



void sync()
{
    const int BUFSIZE=1000;
    char buf[BUFSIZE];
    char* name= getenv("CRO_NAME");
    char* passphrase=getenv("CRO_PASSPHRASE");
    char* enckey=getenv("CRO_ENCKEY");
    const char* req_template = "{\"jsonrpc\": \"2.0\", \"method\": \"sync\", \"params\": [{\"name\":\"%s\", \"passphrase\":\"%s\",\"enckey\":\"%s\"}], \"id\": 1}";
    char req[BUFSIZE];
    sprintf(req, req_template, name, passphrase, enckey);
    char* user_data="i'm user";

    CroResult retcode = cro_jsonrpc_call("./.storage", "ws://localhost:26657/websocket", 0xab, req, buf, sizeof(buf), &progress, user_data);
    if (retcode.result == 0) {
        printf("response: %s\n", buf);
    } else {
        printf("error: %s\n", buf);
    }
}


void jsonrpc( const char* storage, const char* url, const char* request,  char* buf, int buf_len)
{    
    char* user_data="user";    
    CroResult retcode = cro_jsonrpc_call(storage, url, 0xab, request, buf, buf_len, &progress, user_data);
    if (retcode.result == 0) {
        printf("response: %s\n", buf);
    } else {
        printf("error: %s\n", buf);
    }
}



void context_sync()
{
    const int BUFSIZE=1000;
    char buf[BUFSIZE];
    
    char* name= getenv("CRO_NAME");
    char* passphrase=getenv("CRO_PASSPHRASE");
    char* enckey=getenv("CRO_ENCKEY");
    char* mnemonics= getenv("CRO_MNEMONICS");
    char* storage= ".storage";
    char* url= "ws://localhost:26657/websocket";

    const char* req_template = "{\"jsonrpc\": \"2.0\", \"method\": \"sync\", \"params\": [{\"name\":\"%s\", \"passphrase\":\"%s\",\"enckey\":\"%s\"}], \"id\": 1}";
    char req[BUFSIZE];
    sprintf(req, req_template, name, passphrase, enckey);

    const char* user="i'm user";
    const char* wallet_req = "{\"jsonrpc\": \"2.0\", \"method\": \"wallet_list\", \"params\": [], \"id\": 1}";
    const char* wallet_restore_req = "{\"jsonrpc\": \"2.0\", \"method\": \"wallet_restore\", \"params\": [{\"name\":\"%s\", \"passphrase\":\"%s\"}, \"%s\"], \"id\": 2}";
    char tmp[BUFSIZE];
    sprintf(tmp ,wallet_restore_req, name, passphrase, mnemonics);    
    printf("sync with context\n");
    CroJsonRpcPtr rpc= NULL;
    cro_create_jsonrpc(&rpc, storage, url, 0xab, &progress);
    cro_run_jsonrpc(rpc, wallet_req, buf, sizeof(buf), user);        
    printf("response: %s\n", buf);    
    cro_run_jsonrpc(rpc, tmp, buf, sizeof(buf), user);    
    printf("response: %s\n", buf);    
    cro_run_jsonrpc(rpc, req, buf, sizeof(buf),user);
    printf("response: %s\n", buf);
    
    cro_destroy_jsonrpc(rpc);
    printf("OK\n");

}