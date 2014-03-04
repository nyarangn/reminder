/////////////////////////////////////////////////////////////////////////////
/// @file
///
/// Public Data types needed for Reminder operation
///
/// @ingroup Reminder_Library
///
/// @par Copyright (c) 2014 George Nyarangi. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#ifndef REMINDERTYPES_HPP
#define REMINDERTYPES_HPP

#include <string>
#include <map>

#include "quickmail.h"
#include "Types.hpp"

/// todo Need to remove this. Doubt it's being used anywhere
#define MESSAGE_BODY_SIZE 2048

typedef quickmail MailObject_t;
typedef std::map<std::string, std::string> Table_t;

enum ReminderStatus
{
    REMINDER_STATUS_PASS,
    REMINDER_STATUS_FAIL
};

struct Message
{
    std::string recepientEmail;
    std::string senderEmail;
    std::string senderPassword;
    std::string subject;
    std::string body;
    std::string smtpServer;
    uint16_t smtpPort;
};


#endif
