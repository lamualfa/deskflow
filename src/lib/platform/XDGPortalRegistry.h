/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <gio/gio.h>

#include "common/Constants.h"
#include "common/PlatformInfo.h"

namespace deskflow::platform {

/**
 * @brief setAppId Register the app id with xdg-desktop-portal.
 * libportal talks to the portal over GLib's session bus connection, so the
 * registration has to use that same connection: a Qt DBus connection has a
 * different unique bus name and leaves the portal seeing an empty app id.
 * This must run before any other portal call. Sandboxed apps are exempt.
 */
inline void setAppId()
{
  if (isSandboxed()) {
    return;
  }

  g_autoptr(GError) error = nullptr;
  GDBusConnection *const connection = g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &error);
  if (!connection) {
    g_warning("failed to connect to session bus: %s", error->message);
    return;
  }

  GVariantBuilder options;
  g_variant_builder_init(&options, G_VARIANT_TYPE_VARDICT);
  g_autoptr(GVariant) reply = g_dbus_connection_call_sync(
      connection, "org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
      "org.freedesktop.host.portal.Registry", "Register", g_variant_new("(sa{sv})", kRevFqdnName, &options), nullptr,
      G_DBUS_CALL_FLAGS_NONE, -1, nullptr, &error
  );
  if (!reply) {
    g_warning("failed to register portal app id: %s", error->message);
  }
}

} // namespace deskflow::platform
