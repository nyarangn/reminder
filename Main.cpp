#include "quickmail.h"
#include "Reminder.hpp"
#include <string>

#define GMAIL_SMTP_PORT 25

int main (void)
{
	// Instantiate a table object
	Table_t birthdayTable;
	
	//Instantiate and initialize a reminder object
	Reminder myReminder( birthdayTable ); //instantiate birthday table here

	//Read a file
	ReminderStatus status;
	status = myReminder.ReadFile( "input.txt" );
	
	if ( status == REMINDER_STATUS_PASS )
	{
	    //myReminder.DumpTable();
	
	    Message message;
	    message.recepientEmail = "abc@gmail.com";
	    message.senderEmail = "abc@gmail.com";
	    message.senderPassword = "abc";
	    message.subject = "test";
	    message.body = "test";
	    message.smtpServer = "smtp.gmail.com";
	    message.smtpPort = GMAIL_SMTP_PORT;
	    
        myReminder.SendEmail( message );
	}
}
