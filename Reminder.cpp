/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// Class Definition for Reminder
///
/// @ingroup Reminder_Library
///
/// @par Copyright (c) 2014 George Nyarangi. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>  
#include <string.h>
#include <time.h>

#include "ReminderTypes.hpp"
#include "Reminder.hpp"
#include "quickmail.h"
#include "Types.hpp"

#define DEBUG 0
// Define notation for "every month"
const std::string Reminder::EVERY_MONTH = "00";

/////////////////////////////////////////////////////////////////////////////
/// Reminder constructor
/////////////////////////////////////////////////////////////////////////////
Reminder::Reminder()
{
}

/////////////////////////////////////////////////////////////////////////////
/// Overloaded Reminder constructor; Allows 'reminderTable' to be initialized
/// when the 'Reminder' class object is instantiated, i.e.,
/// Reminder myReminder ( myTable )
///
/// @param[in]  reminderTable   Table where all data will be stored    
/////////////////////////////////////////////////////////////////////////////
Reminder::Reminder( Table_t& reminderTable )
{
    this->reminderTable = reminderTable;
}

/////////////////////////////////////////////////////////////////////////////
/// Reminder destructor.
/////////////////////////////////////////////////////////////////////////////
Reminder::~Reminder()
{
}

/////////////////////////////////////////////////////////////////////////////
/// Method for reading an input file
///
/// @param[in]  fileName        Name of the file containing the data 
///
/// @return     ReminderStatus  Status indicating success/failure
///
/// @retval     REMINDER_STATUS_PASS    
///             REMINDER_STATUS_FAIL
/////////////////////////////////////////////////////////////////////////////
ReminderStatus Reminder::ReadFile( const std::string& fileName )
{
    /// todo Refactor ReadFile() to utilize a CSV reader library
    ///      Underscores can be used for now
    std::string line;
    std::string key;
    std::string value;
    ReminderStatus readFileStatus = REMINDER_STATUS_FAIL;

    std::ifstream myFile;
    myFile.open( fileName, std::ifstream::in );

    if ( myFile.is_open() )
    {
        uint32_t datumPosition = 1;
        while ( std::getline( myFile, line, ',') )
        {
            // Remove spaces from strings
            line.erase( std::remove_if( line.begin(), line.end(), ::isspace ), line.end() );
            // Check if datum position is odd
            if (datumPosition % 2) 
            { 
                key = line;
            } 
            else 
            {
                value = line;
                reminderTable.insert( std::pair<std::string, std::string>( key, value ) );
                
                readFileStatus = REMINDER_STATUS_PASS;
            }

            datumPosition++;
        }
        
        myFile.close();
    }
    
    return readFileStatus;
}

/////////////////////////////////////////////////////////////////////////////
/// Method for getting date metadata
///
///param[out]  currentMonth  The current month
///param[out]  currentDay    The current day
///param[out]  longDate      Long date (Www Mmm dd hh:mm:ss yyyy)
/////////////////////////////////////////////////////////////////////////////
void Reminder::GetDate( int16_t& currentMonth, int16_t& currentDay, std::string& longDate )
{
    time_t rawTime;
    time( &rawTime );
    today = localtime( &rawTime );
    currentMonth = today->tm_mon + 1;
    currentDay = today->tm_mday;
    longDate = asctime ( today );
}

/////////////////////////////////////////////////////////////////////////////
/// Method for constructing the email body
///
/// @param[in]  messageBody     Message to be sent
///
/// @param[out] isReadyToBeSent Flag to indicate message status
/////////////////////////////////////////////////////////////////////////////
void Reminder::ConstructEmailBody( std::string& messageBody, bool_t& isReadyToBeSent)
{   
    const char* greeting = "";
    int16_t currentMonth = 0;
    int16_t currentDay = 0;
    std::string longDate = "";
    
    GetDate( currentMonth, currentDay, longDate );
    
    /// @todo Need to send smarter messages
    messageBody.append( "\n" ); 
    messageBody.append( longDate );
    messageBody.append( "-------------------------------------\n" );
    messageBody.append( "Hi:\n\n" );
    messageBody.append( "Here's what's coming up soon:\n\n" );

    for (Table_t::iterator element =  reminderTable.begin(); element != reminderTable.end(); element++ )
    {   
        std::string date = "";
        std::string itemName = element->first;
        
        if (IsElementDue( element, date ) == true )
        {
            isReadyToBeSent = true;
            messageBody.append( '-' + itemName + ", " + date  );
            messageBody.append( "\n" );
#if DEBUG            
            std::cout << "\n-->IsElementDue = true\n" << std::endl;
#endif            
        }
	}
	
    messageBody.append( "\nAdios,\n" );
    messageBody.append( "ReminderBot" );
}

/////////////////////////////////////////////////////////////////////////////
/// Method for sending a notification email
///
/// @param[in]  emailMetadata    Object containing email metadata 
/////////////////////////////////////////////////////////////////////////////
void Reminder::SendEmail( const EmailMetadata& emailMetadata )
{
    bool_t isReadyToBeSent = false;
    std::string messageBody;
    const char* cstrRecepientEmail = emailMetadata.recepientEmail.c_str();
    /// todo Add capability to send email to multiple recepients. Maybe store emails in a 
    ///      linked list?
    const char* cstrCcRecepientEmail = emailMetadata.ccRecepientEmail.c_str();
    const char* cstrSenderEmail = emailMetadata.senderEmail.c_str();
    const char* cstrSubject = emailMetadata.subject.c_str();
    const char* cstrUsername = emailMetadata.senderEmail.c_str();
    const char* cstrPassword = emailMetadata.senderPassword.c_str();
    const char* cstrSmtpServer = emailMetadata.smtpServer.c_str();    
    
    quickmail_initialize();
    mailObject = quickmail_create( cstrSenderEmail, cstrSubject );
    quickmail_add_to( mailObject, cstrRecepientEmail );
    quickmail_add_cc( mailObject, cstrCcRecepientEmail );
    quickmail_set_from( mailObject, cstrSenderEmail );
    quickmail_set_subject( mailObject, cstrSubject );
    quickmail_add_header( mailObject, cstrSubject );
    
    ConstructEmailBody( messageBody, isReadyToBeSent );
#if DEBUG
    std::cout << "message = \n" << messageBody << std::endl;
    std::cout << "\n-->isReadyToBeSent = " << isReadyToBeSent << "\n"<< std::endl;    
#endif

    quickmail_set_body( mailObject, messageBody.c_str() );
        
    // Send the email
    if ( isReadyToBeSent == true )
    {
        quickmail_send( mailObject, cstrSmtpServer, emailMetadata.smtpPort, cstrUsername, cstrPassword );
    }
}

/////////////////////////////////////////////////////////////////////////////
/// Method for tokenizing an element's/item's date
///
/// @param[in]   element    Iterator pointing to the element whose date details
///                         are needed
///
/// @param[out]  dueDay     Day of the month when element is due 
/// @param[out]  dueMonth   Month of the year when the element is due 
/////////////////////////////////////////////////////////////////////////////
void Reminder::TokenizeDate( std::string& dueMonth, std::string& dueDay, Table_t::iterator& element )
{
    std::string itemDate = element->second;
    std::string delimiter = "/";
    size_t delimiterPosition = itemDate.find(delimiter);
    dueMonth = itemDate.substr( 0, delimiterPosition );
    dueDay = itemDate.substr( delimiterPosition + 1, std::string::npos );
}

/////////////////////////////////////////////////////////////////////////////
/// Validate whether an element's notification/reminder is due
///
/// @param[in]   element    Iterator pointing to the element whose date details
///                         are needed
///
/// @param[out]  month      Month of the year when the element is due
/////////////////////////////////////////////////////////////////////////////    
bool_t Reminder::IsElementDue( Table_t::iterator& element, std::string& date )
{
    bool_t isElementDue = false;

    std::string dueMonth;
    std::string dueDay;
    
    TokenizeDate( dueMonth, dueDay, element );
    
    int16_t currentMonth = 0;
    int16_t currentDay = 0;
    std::string longDate = "";
    
    GetDate( currentMonth, currentDay, longDate );
#if DEBUG    
    currentDay = 17;
#endif
    
    // Due this month and notification can happen this month
    if ( atoi(dueDay.c_str()) - NOTIFICATION_HEAD_START > 0 && currentDay == atoi( dueDay.c_str() ) - NOTIFICATION_HEAD_START )
    {
        // Due today because day and month match or it's due every month on this date
        if ( currentMonth == atoi( dueMonth.c_str() ) || dueMonth.compare( EVERY_MONTH ) == 0 )
        {
            isElementDue = true;
            date = std::to_string( currentMonth ) + "/" + dueDay; 
        }
    }
    // Due early next month and notification has to happen late this month
    else
    {
        if ( currentDay == DAYS_IN_A_MONTH + atoi(dueDay.c_str()) - NOTIFICATION_HEAD_START )
        {
            if ( currentMonth + 1 == atoi( dueMonth.c_str() ) || dueMonth.compare( EVERY_MONTH ) == 0 )
            {          
                isElementDue = true;
                date = std::to_string( currentMonth +1 ) + "/" + dueDay; 
            }
        }
    }
       
    return isElementDue;
}
                                                            
/////////////////////////////////////////////////////////////////////////////
/// Method for dumping data from the tables
/////////////////////////////////////////////////////////////////////////////
void Reminder::DumpTable( void )
{
    for ( Table_t::iterator element =  reminderTable.begin(); element != reminderTable.end(); element++ )
    {
        std::cout << "reminderTable[" << element->first << "] = " << element->second << std::endl;
    }
}
