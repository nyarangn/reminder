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
///
/// @test        Reminder_ReadFile.cpp
/////////////////////////////////////////////////////////////////////////////
ReminderStatus Reminder::ReadFile( const std::string& fileName )
{
    std::string line;
    std::string key;
    std::string value;
    ReminderStatus readFileStatus = REMINDER_STATUS_FAIL;

    /// @future Refactor ReadFile() to utilize a CSV reader library
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
/// Method for adding an email recepient to the "to" field
///
/// @param[in]  additionalRecepient  Recepient to be addedd
/////////////////////////////////////////////////////////////////////////////
void Reminder::AddRecepient( std::string additionalRecepient )
{
    const char* cstrAdditionalRecepient = additionalRecepient.c_str();
    quickmail_add_to ( mailObject, cstrAdditionalRecepient );
}  

/////////////////////////////////////////////////////////////////////////////
/// Method for getting date metadata
///
///param[in, out]  currentMonth  The current month
///param[in, out]  currentDay    The current day
///param[in, out]  longDate      Long date (Www Mmm dd hh:mm:ss yyyy)
/////////////////////////////////////////////////////////////////////////////
void Reminder::GetDate( int16_t& currentMonth, int16_t& currentDay, std::string& longDate )
{
    time_t rawTime;
    time( &rawTime );
    today = localtime( &rawTime );
    currentMonth = today->tm_mon + 1;
    currentDay = today->tm_mday;
    longDate = asctime ( today );
    //strcpy( *longDate, asctime ( today ));
}

/////////////////////////////////////////////////////////////////////////////
/// Method for constructing the email body
///
/// @param[in]  additionalRecepient  Recepient to be addedd
/////////////////////////////////////////////////////////////////////////////
void Reminder::ConstructEmailBody( std::string& messageBody, bool_t& readyToBeSent)
{   
    const char* greeting = "";
    int16_t currentMonth = 0;
    int16_t currentDay = 0;
    std::string longDate = "";
    GetDate( currentMonth, currentDay, longDate );

    messageBody.append( "\n" ); 
    messageBody.append( longDate );
    messageBody.append( "-------------------------------------\n" );
    messageBody.append( "Hi:\n\n" );
    messageBody.append( "The following items are due soon:\n\n" );
    
    for (Table_t::iterator location =  reminderTable.begin(); location != reminderTable.end(); location++ )
    {   
    	/// todo Pull this section out into its own function (TokenizeDueDate())
        std::string itemDate = location->second;
        std::string itemName = location->first;
        std::string delimiter = "/";
        size_t delimiterPosition = itemDate.find(delimiter);
        std::string dueMonth = itemDate.substr( 0, delimiterPosition );
        std::string dueDay = itemDate.substr( delimiterPosition + 1, std::string::npos );

          //std::cout << " here2 -->"  << DAYS_IN_A_MONTH + atoi( dueDay.c_str() ) << std::endl;
        if ( currentDay == atoi( dueDay.c_str() ) - NOTIFICATION_HEAD_START && dueMonth.compare( "00" ) == 0 || 
             ( ( atoi(dueDay.c_str()) - NOTIFICATION_HEAD_START < 0 ) && 
               ( currentDay == DAYS_IN_A_MONTH + atoi(dueDay.c_str()) - NOTIFICATION_HEAD_START ) ) || 
              // Check is b
             ( currentMonth == atoi( dueMonth.c_str() )  && currentDay == atoi( dueDay.c_str() ) - NOTIFICATION_HEAD_START ) )
        {
            // Bill is due this month and notification can be done this month
            if ( dueMonth.compare( "00" ) == 0 &&  atoi(dueDay.c_str()) - NOTIFICATION_HEAD_START > 0)
            {
                itemDate = std::to_string( currentMonth ) + "/" + dueDay;
                messageBody.append( itemName + " is due on " +  itemDate  );
                readyToBeSent = 1;
            }
            // Bill is due early next month and notification has to be done late this month
            else if ( dueMonth.compare( "00" ) == 0 && atoi(dueDay.c_str()) - NOTIFICATION_HEAD_START < 0 ) 
            {
                itemDate = std::to_string( currentMonth + 1 ) + "/" + dueDay;
                messageBody.append( itemName + " is due on " +  itemDate  );
                readyToBeSent = 1;
            }
            // Bill month has been specified
            else
            {
               messageBody.append( itemName + " is due on " +  itemDate  );
               readyToBeSent = 1;
            }

            messageBody.append("\n");
        }
	}
    
    messageBody.append( "\n" );
    messageBody.append( "Adios,\n" );
    messageBody.append( "ReminderBot" );
}

/////////////////////////////////////////////////////////////////////////////
/// Method for sending a notification email
///
/// @param[in]  message    Message object containing email details 
/////////////////////////////////////////////////////////////////////////////
void Reminder::SendEmail( const Message& message )
{
    bool_t readyToBeSent = 0;
    std::string messageBody;
    const char* cstrRecepientEmail = message.recepientEmail.c_str();
    const char* cstrSenderEmail = message.senderEmail.c_str();
    const char* cstrSubject = message.subject.c_str();
    const char* cstrUsername = message.senderEmail.c_str();
    const char* cstrPassword = message.senderPassword.c_str();
    const char* cstrSmtpServer = message.smtpServer.c_str();    
    
    quickmail_initialize();
    mailObject = quickmail_create( cstrSenderEmail, cstrSubject );
    quickmail_add_to( mailObject, cstrRecepientEmail );
    quickmail_set_from( mailObject, cstrSenderEmail );
    quickmail_set_subject( mailObject, cstrSubject );
    quickmail_add_header( mailObject, cstrSubject );
    
    ConstructEmailBody( messageBody, readyToBeSent );
    std::cout << "message = \n" << messageBody << std::endl;
    quickmail_set_body( mailObject, messageBody.c_str() );

    // Send the email
    if ( readyToBeSent == 1 )
    {
        //quickmail_send( mailObject, cstrSmtpServer, message.smtpPort, cstrUsername, cstrPassword );
    }
    //test
}
                                                                      
/////////////////////////////////////////////////////////////////////////////
/// Method for dumping data from the tables
/////////////////////////////////////////////////////////////////////////////
void Reminder::DumpTable( void )
{
    for ( Table_t::iterator location =  reminderTable.begin(); location != reminderTable.end(); location++ )
    {
        std::cout << "reminderTable[" << location->first << "] = " << location->second << std::endl;
    }
}
