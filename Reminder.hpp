/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// Class Definition for Reminder
///
/// @ingroup Reminder_Library
///
/// @par Copyright (c) 2014 George Nyarangi. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


// Need method to send email
// need method to construct message

#ifndef REMINDER_HPP
#define REMINDER_HPP

#include <string>
#include <map>

#include "ReminderTypes.hpp"
#include "Types.hpp"

class Reminder
{
    public:

        Reminder();
        Reminder( Table_t& reminderTable );
        ~Reminder();
                
        ReminderStatus ReadFile( const std::string& fileName );
        void AddRecepient (std::string additionalRecepient );
        void SendEmail( const Message& message );
        
        void DumpTable( void );
        
    private:
        struct tm* today;
        Table_t reminderTable;
        MailObject_t mailObject;
        //This needs to be part of user input
        static const int16_t NOTIFICATION_HEAD_START = 5;
        static const int16_t DAYS_IN_A_MONTH = 30;
        void GetDate( int16_t& currentMonth, int16_t& currentDay, std::string& longDate );
        void ConstructEmailBody( std::string& messageBody, bool_t& readyToBeSent );
};

#endif
