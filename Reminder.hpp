/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// Class Definition for Reminder
///
/// @ingroup Reminder_Library
///
/// @par Copyright (c) 2014 George Nyarangi. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

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
        void SendEmail( const EmailMetadata& emailMetadata );
        
        void DumpTable( void );
        
    private:
    
        struct tm* today;
        Table_t reminderTable;
        MailObject_t mailObject;
        static const std::string EVERY_MONTH;
        /// todo Make the head start a part of user input
        static const int16_t NOTIFICATION_HEAD_START = 5;
        static const int16_t DAYS_IN_A_MONTH = 30;
        
        bool_t IsElementDue( Table_t::iterator& element, std::string& date );
        void TokenizeDate( std::string& dueMonth, std::string& dueDay, Table_t::iterator& element );
        void GetDate( int16_t& currentMonth, int16_t& currentDay, std::string& longDate );
        void ConstructEmailBody( std::string& messageBody, bool_t& isReadyToBeSent );
};

#endif
