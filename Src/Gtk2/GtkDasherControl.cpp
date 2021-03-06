// GtkDasherControl.cpp
//
// Copyright (c) 2008 The Dasher Team
//
// This file is part of Dasher.
//
// Dasher is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Dasher is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dasher; if not, write to the Free Software 
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "../Common/Common.h"

#include "DasherControl.h"
#include "GtkDasherControl.h"
#include "custom_marshal.h"
#include "dasher_editor.h"

struct _GtkDasherControlPrivate {
  CDasherControl *pControl;
  DasherEditor *pEditor;
};

typedef struct _GtkDasherControlPrivate GtkDasherControlPrivate;

  // Signals that this control can emit

enum {
  DASHER_CHANGED,
  DASHER_STOP,
  DASHER_EDIT_INSERT,
  DASHER_EDIT_DELETE,
  DASHER_EDIT_CONVERT,
  DASHER_EDIT_PROTECT,
  DASHER_CONTROL,
  DASHER_REQUEST_SETTINGS,
  DASHER_LOCK_INFO,
  DASHER_MESSAGE,
  SIGNAL_NUM
};

#define GTK_DASHER_CONTROL_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), GTK_DASHER_TYPE_CONTROL, GtkDasherControlPrivate));

/* TODO: is it still sensible to derive from VBox, given that its just a cnavas now*/
G_DEFINE_TYPE(GtkDasherControl, gtk_dasher_control, GTK_TYPE_VBOX);

static void gtk_dasher_control_finalize(GObject * pObject);

static guint gtk_dasher_control_signals[SIGNAL_NUM]; /* TODO: initialise this? */

static void 
gtk_dasher_control_class_init(GtkDasherControlClass *pClass) {
  g_type_class_add_private(pClass, sizeof(GtkDasherControlPrivate));

  GObjectClass *pObjectClass = (GObjectClass *) pClass;
  pObjectClass->finalize = gtk_dasher_control_finalize;
   
  gtk_dasher_control_signals[DASHER_CHANGED] = g_signal_new("dasher_changed", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_changed), NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);

  gtk_dasher_control_signals[DASHER_STOP] = g_signal_new("dasher_stop", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_stop), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  gtk_dasher_control_signals[DASHER_EDIT_INSERT] = g_signal_new("dasher_edit_insert", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_edit_insert), NULL, NULL, g_cclosure_user_marshal_VOID__STRING_INT, G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_INT);

  gtk_dasher_control_signals[DASHER_EDIT_DELETE] = g_signal_new("dasher_edit_delete", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_edit_delete), NULL, NULL, g_cclosure_user_marshal_VOID__STRING_INT, G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_INT);

 gtk_dasher_control_signals[DASHER_EDIT_CONVERT] = g_signal_new("dasher_edit_convert", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_edit_convert), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
 
 gtk_dasher_control_signals[DASHER_EDIT_PROTECT] = g_signal_new("dasher_edit_protect", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_edit_protect), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  gtk_dasher_control_signals[DASHER_CONTROL] = g_signal_new("dasher_control", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_control), NULL, NULL, g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);

//   gtk_dasher_control_signals[DASHER_CONTROL] = g_signal_new("key_press_event", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, key_press_event), NULL, NULL, gtk_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, GDK_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

//   gtk_dasher_control_signals[DASHER_CONTROL] = g_signal_new("key_release_event", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, key_release_event), NULL, NULL, gtk_marshal_BOOLEAN__POINTER, G_TYPE_BOOLEAN, 1, GDK_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

  gtk_dasher_control_signals[DASHER_REQUEST_SETTINGS] = g_signal_new("dasher_request_settings", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_request_settings), NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

  gtk_dasher_control_signals[DASHER_LOCK_INFO] = g_signal_new("dasher_lock_info", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_lock), NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

  gtk_dasher_control_signals[DASHER_MESSAGE] = g_signal_new("dasher_message", G_TYPE_FROM_CLASS(pClass), static_cast < GSignalFlags > (G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION), G_STRUCT_OFFSET(GtkDasherControlClass, dasher_message), NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

  pClass->dasher_changed = NULL;
  pClass->dasher_stop = NULL;
  pClass->dasher_edit_insert = NULL;
  pClass->dasher_edit_delete = NULL;
  pClass->dasher_edit_convert = NULL;
  pClass->dasher_edit_protect = NULL;
  pClass->dasher_control = NULL;
  pClass->dasher_request_settings = NULL;  
  pClass->dasher_lock = NULL;
  pClass->dasher_message = NULL;
  //  pClass->key_press_event = gtk_dasher_control_default_key_press_handler;
  // pClass->key_release_event = gtk_dasher_control_default_key_release_handler;
}

static void 
gtk_dasher_control_init(GtkDasherControl *pDasherControl) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pDasherControl);

  pPrivate->pControl = new CDasherControl(&(pDasherControl->box), pDasherControl);

//   g_signal_connect(G_OBJECT(pDasherControl), "key-press-event", G_CALLBACK(gtk_dasher_control_default_key_press_handler), pPrivate->pControl);
//   g_signal_connect(G_OBJECT(pDasherControl), "key-release-event", G_CALLBACK(gtk_dasher_control_default_key_release_handler), pPrivate->pControl);
}

void gtk_dasher_control_set_editor(GtkDasherControl *pDasherControl, DasherEditor *pEditor) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pDasherControl);
  
  DASHER_ASSERT (pPrivate->pEditor == NULL);
  DASHER_ASSERT (pEditor != NULL);
  pPrivate->pEditor = pEditor;
}

GtkTextBuffer *gtk_dasher_control_game_text_buffer(GtkDasherControl *pDasherControl) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pDasherControl);
  return dasher_editor_game_text_buffer(pPrivate->pEditor);
}

void gtk_dasher_control_set_game_mode(GtkDasherControl *pControl, bool bOn) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  if (bOn)
    pPrivate->pControl->EnterGameMode(NULL);
  else
    pPrivate->pControl->LeaveGameMode();
}

bool gtk_dasher_control_get_game_mode(GtkDasherControl *pControl) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return pPrivate->pControl->GetGameModule() ? true : false;
}

static void 
gtk_dasher_control_finalize(GObject *pObject) {
  GtkDasherControl *pDasherControl = GTK_DASHER_CONTROL(pObject);

  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pDasherControl);


  /* TODO: Check that this actually gets called correctly */

  /* TODO: Should just call constructor - this should just be a lightweight wrapper class */
  //ACL not sure what that's about...?
  //The following comment taken from the old ShutdownTimer, which we need to do now:
    // TODO: Figure out how to implement this - at the moment it's done
    // through a return value from the timer callback, but it would be
    // nicer to prevent any further calls as soon as the shutdown signal
    // has been receieved.
  pPrivate->pControl->WriteTrainFileFull();

  delete pPrivate->pControl;
  //  g_free(pDasherControl->private_data);
}

GtkWidget *
gtk_dasher_control_new() {
  return GTK_WIDGET(g_object_new(GTK_DASHER_TYPE_CONTROL, NULL));
}

void 
gtk_dasher_control_set_parameter_bool(GtkDasherControl *pControl, int iParameter, bool bValue) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->SetBoolParameter(iParameter, bValue);
}

void 
gtk_dasher_control_set_parameter_long(GtkDasherControl *pControl, int iParameter, long lValue) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->SetLongParameter(iParameter, lValue);
}

void 
gtk_dasher_control_set_parameter_string(GtkDasherControl *pControl, int iParameter, const char *szValue) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->SetStringParameter(iParameter, szValue);
}

bool
gtk_dasher_control_get_parameter_bool(GtkDasherControl *pControl, int iParameter) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return pPrivate->pControl->GetBoolParameter(iParameter);
}

long 
gtk_dasher_control_get_parameter_long(GtkDasherControl *pControl, int iParameter) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return pPrivate->pControl->GetLongParameter(iParameter);
}

void 
gtk_dasher_control_reset_parameter(GtkDasherControl *pControl, int iParameter) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->ResetParameter(iParameter);
}

const char *
gtk_dasher_control_get_parameter_string(GtkDasherControl *pControl, int iParameter) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return pPrivate->pControl->GetStringParameter(iParameter).c_str();
}

GArray *
gtk_dasher_control_get_allowed_values(GtkDasherControl *pControl, int iParameter) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return pPrivate->pControl->GetAllowedValues(iParameter);
};

void 
gtk_dasher_control_train(GtkDasherControl *pControl, const gchar *szFilename) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->ImportTrainingText(szFilename);
};

const gchar *
gtk_dasher_control_get_context(GtkDasherControl *pControl, unsigned int iStart, unsigned int iLength) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return dasher_editor_get_context(pPrivate->pEditor, iStart, iLength);
}

const gchar *
gtk_dasher_control_get_all_text(GtkDasherControl *pControl) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return dasher_editor_get_all_text(pPrivate->pEditor);
}

void gtk_dasher_control_clear_all_context(GtkDasherControl *pControl) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  dasher_editor_clear(pPrivate->pEditor);
}

void 
gtk_dasher_control_set_buffer(GtkDasherControl *pControl, int iOffset) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->SetBuffer(iOffset);
}

void 
gtk_dasher_control_set_offset(GtkDasherControl *pControl, int iOffset) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->SetOffset(iOffset);
}

gint gtk_dasher_control_ctrl_move(GtkDasherControl *pControl, bool bForwards, CControlManager::EditDistance dist) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return dasher_editor_ctrl_move(pPrivate->pEditor, bForwards, dist);
}

gint gtk_dasher_control_ctrl_delete(GtkDasherControl *pControl, bool bForwards, CControlManager::EditDistance dist) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return dasher_editor_ctrl_delete(pPrivate->pEditor, bForwards, dist);
}

void 
gtk_dasher_control_external_key_down(GtkDasherControl *pControl, int iKeyVal) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->ExternalKeyDown(iKeyVal);
}

void 
gtk_dasher_control_external_key_up(GtkDasherControl *pControl, int iKeyVal) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->ExternalKeyUp(iKeyVal);
}

void 
gtk_dasher_user_log_new_trial(GtkDasherControl * pControl) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->UserLogNewTrial();
}

void 
gtk_dasher_control_set_focus(GtkDasherControl * pControl){
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->SetFocus();
}

// gboolean 
// gtk_dasher_control_default_key_press_handler(GtkDasherControl *pDasherControl, GdkEventKey *pEvent, gpointer data){
//   static_cast<CDasherControl *>(data)->KeyPressEvent(pEvent);
//   return FALSE;
// }

// gboolean 
// gtk_dasher_control_default_key_release_handler(GtkDasherControl *pDasherControl, GdkEventKey *pEvent, gpointer data) {
//  static_cast<CDasherControl *>(data)->KeyReleaseEvent(pEvent);
//  return FALSE;
// }

gboolean 
gtk_dasher_control_get_module_settings(GtkDasherControl * pControl, const gchar *szModule, SModuleSettings **pSettings, gint *iCount) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return pPrivate->pControl->GetModuleSettings(szModule, pSettings, iCount);
}

void 
gtk_dasher_control_force_pause(GtkDasherControl *pControl) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  pPrivate->pControl->GetActiveInputMethod()->pause();
}

const char *
gtk_dasher_control_cl_set(GtkDasherControl *pControl, const gchar *szKey, const gchar *szValue) {
  GtkDasherControlPrivate *pPrivate = GTK_DASHER_CONTROL_GET_PRIVATE(pControl);
  return pPrivate->pControl->ClSet(szKey, szValue);
}
