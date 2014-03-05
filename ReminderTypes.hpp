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

typedef quickmail MailObject_t;
typedef std::map<std::string, std::string> Table_t;

enum ReminderStatus
{
    REMINDER_STATUS_PASS,
    REMINDER_STATUS_FAIL
};

struct EmailMetadata
{
    std::string recepientEmail;
    std::string ccRecepientEmail;
    std::string senderEmail;
    std::string senderPassword;
    std::string subject;
    std::string smtpServer;
    uint16_t smtpPort;
};

#endif
