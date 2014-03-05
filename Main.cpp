#include "quickmail.h"
#include "Reminder.hpp"
#include <string>

#define GMAIL_SMTP_PORT 25

int main (void)
{
	// Instantiate table objects
	Table_t billsTable;
	Table_t birthdayTable;
	
	//Instantiate and initialize reminder objects
	Reminder billReminder( billsTable ); 
    Reminder birthdayReminder( birthdayTable ); 
    
    // Define email metadata
    EmailMetadata emailMetadata;
    emailMetadata.recepientEmail = "john.doe@gmail.com";
    emailMetadata.ccRecepientEmail = "jane@doe.com";
    emailMetadata.senderEmail = "reminder.bot@gmail.com";
    emailMetadata.subject = "Bill Reminder";
    emailMetadata.senderPassword = "password";
    emailMetadata.smtpServer = "smtp.gmail.com";
    emailMetadata.smtpPort = GMAIL_SMTP_PORT;    

    // send reminders
	ReminderStatus billStatus;
	billStatus = billReminder.ReadFile( "bills.txt" );
	if ( billStatus == REMINDER_STATUS_PASS )
	{    
        billReminder.SendEmail( emailMetadata );
	}
	
	ReminderStatus birthdayStatus;
	birthdayStatus = birthdayReminder.ReadFile( "birthdays.txt" );
    emailMetadata.subject = "Birthday Reminder";
    if ( birthdayStatus == REMINDER_STATUS_PASS )
    {    
        birthdayReminder.SendEmail( emailMetadata );
    }
}
