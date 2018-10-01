// This file is part of dbus-asio
// Copyright 2018 Brightsign LLC
//
// This library is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, version 3, or at your
// option any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// The GNU Lesser General Public License version 3 is included in the
// file named COPYING. If you do not have this file see
// <http://www.gnu.org/licenses/>.

#ifndef DBUS_H
#define DBUS_H

// Core elements and types
#include "dbus_log.h"
#include "dbus_platform.h"
#include "dbus_utils.h"
#include "dbus_validation.h"

// Marshall/unmarshalling types
#include "dbus_type.h"
#include "dbus_type_array.h"
#include "dbus_type_base.h"
#include "dbus_type_boolean.h"
#include "dbus_type_byte.h"
#include "dbus_type_dictentry.h"
#include "dbus_type_double.h"
#include "dbus_type_int16.h"
#include "dbus_type_int32.h"
#include "dbus_type_int64.h"
#include "dbus_type_objectpath.h"
#include "dbus_type_signature.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint16.h"
#include "dbus_type_uint32.h"
#include "dbus_type_uint64.h"
#include "dbus_type_variant.h"

// Functionality
#include "dbus_auth.h"
#include "dbus_matchrule.h"
#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_native.h"
#include "dbus_transport.h"

// Introspective library
#include "dbus_introspectable.h"

#endif // DBUS_H
