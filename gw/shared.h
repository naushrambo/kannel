/*
 * shared.h - utility functions shared by Kannel boxes
 *
 * The functions declared here are not part of any box in particular, but
 * are quite specific to Kannel, so they are not suitable for gwlib, either.
 *
 * Lars Wirzenius
 */

#ifndef SHARED_H
#define SHARED_H


#include "gwlib/gwlib.h"
#include "msg.h"

#define CATENATE_UDH_LEN 5
#define INFINITE_TIME -1

/*
 * Program status. Set this to shutting_down to make read_from_bearerbox
 * return even if the bearerbox hasn't closed the connection yet.
 */
extern enum program_status {
    starting_up,
    running,
    shutting_down
} program_status;


/*
 * Return an octet string with information about Kannel version,
 * operating system, and libxml version. The caller must take care to
 * destroy the string when done.
 */
Octstr *version_report_string(const char *boxname);


/*
 * Output the information returned by version_report_string to the log
 * files.
 */
void report_versions(const char *boxname);


/*
 * Open a connection to the bearerbox.
 */
void connect_to_bearerbox(Octstr *host, int port, int ssl, Octstr *our_host);


/*
 * Try to open a connection to the bearerbox and return the connection pointer.
 */
Connection *get_connect_to_bearerbox(Octstr *host, int port, int ssl, Octstr *our_host);


/*
 * Close connection to the bearerbox.
 */
void close_connection_to_bearerbox(void);


/*
 * Receive Msg from bearerbox. Unblock the call when the given
 * timeout for conn_wait() is reached. Use a negative value, 
 * ie. -1 for an infinite blocking, hence no timeout applies. 
 * Return NULL if connection broke or timed out.
 */
Msg *read_from_bearerbox(double seconds);


/*
 * Send an Msg to the bearerbox, and destroy the Msg.
 */
void write_to_bearerbox(Msg *msg);


/*
 * Delivers a SMS to the bearerbox and returns an error code: 0 if
 * successfull. -1 if transfer failed.
 *
 * Note: Message is only destroyed if sucessfully delivered!
 */
int deliver_to_bearerbox(Msg *msg);

     
/*
 * Validates an OSI date.
 */
Octstr *parse_date(Octstr *date);

/*
 * 
 * Split an SMS message into smaller ones.
 * 
 * The original SMS message is represented as an Msg object, and the
 * resulting list of smaller ones is represented as a List of Msg objects.
 * A plain text header and/or footer can be added to each part, and an
 * additional suffix can be added to each part except the last one.
 * Optionally, a UDH prefix can be added to each part so that phones
 * that understand this prefix can join the messages into one large one
 * again. At most `max_messages' parts will be generated; surplus text
 * from the original message will be silently ignored.
 * 
 * If the original message has UDH, they will be duplicated in each part.
 * It is an error to use catenation and UDH together, or catenation and 7
 * bit mode toghether; in these cases, catenation is silently ignored.
 * 
 * If `catenate' is true, `msg_sequence' is used as the sequence number for
 * the logical message. The catenation UDH contain three numbers: the
 * concatenated message reference, which is constant for all parts of
 * the logical message, the total number of parts in the logical message,
 * and the sequence number of the current part.
 *
 * Note that `msg_sequence' must have a value in the range 0..255.
 * 
 * `max_octets' gives the maximum number of octets in on message, including
 * UDH, and after 7 bit characters have been packed into octets.
 */
List *sms_split(Msg *orig, Octstr *header, Octstr *footer, 
                Octstr *nonlast_suffix, Octstr *split_chars, int catenate, 
                unsigned long msg_sequence, int max_messages, int max_octets);

#endif






