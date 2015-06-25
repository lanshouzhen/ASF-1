﻿/*
 * IncFile1.h
 *
 * Created: 2014-12-18 오후 1:40:38
 *  Author: kevin
 */ 


#ifndef PUBNUB_H_
#define PUBNUB_H_

#include "socket/include/socket.h"


#define PUBNUB_ORIGIN	"pubsub.pubnub.com"
#define PUBNUB_ORIGIN_PORT	80

#define PUBNUB_BUF_MAXLEN	256
#define PUBNUB_REPLY_MAXLEN	512

#define PUBNUB_CTX_MAX	2


enum pubnub_trans {
	/** No transaction at all */
	PBTT_NONE,
	/** Subscribe transaction */
	PBTT_SUBSCRIBE,
	/** Publish transaction */
	PBTT_PUBLISH,
	/** Leave (channel(s)) transaction */
	PBTT_LEAVE,
};

enum pubnub_state {
	PS_IDLE,
	PS_WAIT_DNS,
	PS_WAIT_CONNECT,
	PS_WAIT_SEND,
	PS_WAIT_RECV,
	PS_RECV,
};

enum pubnub_res {
/** Success. Transaction finished successfully. */
    PNR_OK,
    /** Time out before the request has completed. */
    PNR_TIMEOUT,
    /** Communication error (network or HTTP response format). */
    PNR_IO_ERROR,
    /** HTTP error. */
    PNR_HTTP_ERROR,
    /** Unexpected input in received JSON. */
    PNR_FORMAT_ERROR,
    /** Request cancelled by user. */
    PNR_CANCELLED,
    /** Transaction started. Await the outcome via process message. */
    PNR_STARTED,
    /** Transaction (already) ongoing. Can't start a new transaction. */
    PNR_IN_PROGRESS,
    /** Receive buffer (from previous transaction) not read, new
	subscription not allowed.
    */
    PNR_RX_BUFF_NOT_EMPTY,
    /** The buffer is to small. Increase #PUBNUB_BUF_MAXLEN.
    */
    PNR_TX_BUFF_TOO_SMALL
};


struct pubnub {
	const char *publish_key, *subscribe_key;
	const char *uuid, *auth;
	char timetoken[64];
	
	/** The result of the last Pubnub transaction */
	enum pubnub_res last_result;
	
	/** Network communication state */
	enum pubnub_trans trans;
	enum pubnub_state state;
	SOCKET tcp_socket;
	union { char url[PUBNUB_BUF_MAXLEN]; char line[PUBNUB_BUF_MAXLEN]; } http_buf;
	int http_code;
	unsigned http_buf_len;
	unsigned http_content_len;
	unsigned http_content_remaining_len;
	bool http_chunked;
	char http_reply[PUBNUB_REPLY_MAXLEN];
	
    /* These in-string offsets are used for yielding messages received
     * by subscribe - the beginning of last yielded message and total
     * length of message buffer, and the same for channels.
     */
    unsigned short msg_ofs, msg_end, chan_ofs, chan_end;
};


typedef struct pubnub pubnub_t;


/** Returnes a context for the given index. Contexts are statically allocated by the PubNub library
and this is the only to get a pointer to one of them.
*/
pubnub_t *pubnub_get_ctx(uint8_t index);

/** Init the PubNub Client API
*/
void pubnub_init(pubnub_t *pb, const char *publish_key, const char *subscribe_key);
bool pubnub_publish(pubnub_t *pb, const char *channel, const char *message);
bool pubnub_subscribe(pubnub_t *pb, const char *channel);
char const *pubnub_get(pubnub_t *pb);

void handle_dns_found(const char *name, uint32_t hostip);
void handle_tcpip(SOCKET sock, uint8_t u8Msg, void *pvMsg);


#endif /* PUBNUB_H_ */