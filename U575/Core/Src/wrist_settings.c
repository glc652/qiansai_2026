#include "wrist_settings.h"

#include <stddef.h>
#include <stdint.h>

#define WRIST_SETTINGS_ROW_Y(index) (36 + ((index) * 39))
#define WRIST_SETTINGS_TXT_OPEN      "\xE6""\x89""\x93""\xE5""\xBC""\x80"
#define WRIST_SETTINGS_TXT_CLOSE     "\xE5""\x85""\xB3""\xE9""\x97""\xAD"
#define WRIST_SETTINGS_TXT_TITLE     "\xE8""\xAE""\xBE""\xE7""\xBD""\xAE"
#define WRIST_SETTINGS_TXT_HEART_KEY "\xE5""\x90""\xAF""\xE7""\x94""\xA8""\xE5""\xBF""\x83""\xE7""\x8E""\x87""\xE6""\x8C""\x89""\xE9""\x94""\xAE"
#define WRIST_SETTINGS_TXT_VIBRATION "\xE6""\x98""\xAF""\xE5""\x90""\xA6""\xE6""\x89""\x93""\xE5""\xBC""\x80""\xE6""\x8C""\xAF""\xE5""\x8A""\xA8"
#define WRIST_SETTINGS_TXT_NIGHT     "\xE5""\x88""\x87""\xE6""\x8D""\xA2""\xE6""\x97""\xA5""\xE5""\xA4""\x9C""\xE6""\xA8""\xA1""\xE5""\xBC""\x8F"
#define WRIST_SETTINGS_TXT_DAY       "\xE7""\x99""\xBD""\xE5""\xA4""\xA9"
#define WRIST_SETTINGS_TXT_DARK      "\xE9""\xBB""\x91""\xE5""\xA4""\x9C"
#define WRIST_SETTINGS_TXT_RESTORE   "\xE6""\x81""\xA2""\xE5""\xA4""\x8D""\xE9""\xBB""\x98""\xE8""\xAE""\xA4""\xE5""\x80""\xBC"

static uint8_t s_saved_heart_key_enabled = 1U;
static uint8_t s_saved_vibration_enabled = 1U;
static uint8_t s_saved_night_mode = 0U;

static lv_color_t WristSettings_SurfaceColor(const WristSettingsView_t *view)
{
  (void)view;
  return (s_saved_night_mode != 0U) ? lv_color_hex(0x10233A) : lv_color_hex(0xFFFFFF);
}

static lv_color_t WristSettings_TextColor(const WristSettingsView_t *view)
{
  (void)view;
  return (s_saved_night_mode != 0U) ? lv_color_hex(0xF7FBFF) : lv_color_hex(0x102B6B);
}

static lv_color_t WristSettings_BorderColor(const WristSettingsView_t *view)
{
  (void)view;
  return (s_saved_night_mode != 0U) ? lv_color_hex(0x245C96) : lv_color_hex(0xDCEBFF);
}

static void WristSettings_RestoreEditDefaults(WristSettingsView_t *view)
{
  if (view == NULL)
  {
    return;
  }

  view->edit_heart_key_enabled = 1U;
  view->edit_vibration_enabled = 1U;
  view->edit_night_mode = 0U;
}

static const char *WristSettings_ValueText(const WristSettingsView_t *view, uint8_t row)
{
  if (view == NULL)
  {
    return "";
  }

  if (row == 0U)
  {
    return (view->edit_heart_key_enabled != 0U) ? WRIST_SETTINGS_TXT_OPEN : WRIST_SETTINGS_TXT_CLOSE;
  }
  if (row == 1U)
  {
    return (view->edit_vibration_enabled != 0U) ? WRIST_SETTINGS_TXT_OPEN : WRIST_SETTINGS_TXT_CLOSE;
  }
  if (row == 2U)
  {
    return (view->edit_night_mode != 0U) ? WRIST_SETTINGS_TXT_DARK : WRIST_SETTINGS_TXT_DAY;
  }
  return "";
}

static void WristSettings_Refresh(WristSettingsView_t *view)
{
  uint8_t i;

  if ((view == NULL) || (view->overlay == NULL))
  {
    return;
  }

  lv_obj_set_style_bg_color(view->overlay, (s_saved_night_mode != 0U) ? lv_color_hex(0x000000) : lv_color_hex(0x102B6B), 0);

  for (i = 0U; i < WRIST_SETTINGS_ROW_COUNT; i++)
  {
    if (view->rows[i] != NULL)
    {
      lv_obj_set_style_border_width(view->rows[i], (i == view->selected) ? 2 : 0, 0);
      lv_obj_set_style_border_color(view->rows[i], lv_color_hex(0x1877F2), 0);
      lv_obj_set_style_border_opa(view->rows[i], (i == view->selected) ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
      lv_obj_invalidate(view->rows[i]);
    }
    if (view->labels[i] != NULL)
    {
      lv_obj_set_style_text_color(view->labels[i], WristSettings_TextColor(view), 0);
    }
    if (view->values[i] != NULL)
    {
      lv_label_set_text(view->values[i], WristSettings_ValueText(view, i));
      lv_obj_set_style_text_color(view->values[i], lv_color_hex(0x1877F2), 0);
    }
  }

  if (view->panel != NULL)
  {
    lv_obj_set_style_bg_color(view->panel, WristSettings_SurfaceColor(view), 0);
    lv_obj_set_style_border_color(view->panel, WristSettings_BorderColor(view), 0);
    lv_obj_invalidate(view->panel);
  }
  if (view->title != NULL)
  {
    lv_obj_set_style_text_color(view->title, WristSettings_TextColor(view), 0);
  }
}

static void WristSettings_ApplyRow(WristSettingsView_t *view, uint8_t row)
{
  if (view == NULL)
  {
    return;
  }

  if (row == 0U)
  {
    view->edit_heart_key_enabled = (view->edit_heart_key_enabled == 0U) ? 1U : 0U;
  }
  else if (row == 1U)
  {
    view->edit_vibration_enabled = (view->edit_vibration_enabled == 0U) ? 1U : 0U;
  }
  else if (row == 2U)
  {
    view->edit_night_mode = (view->edit_night_mode == 0U) ? 1U : 0U;
    s_saved_night_mode = view->edit_night_mode;
  }
  else
  {
    WristSettings_RestoreEditDefaults(view);
    s_saved_night_mode = view->edit_night_mode;
  }
  WristSettings_Refresh(view);
}

static void WristSettings_RowEventCb(lv_event_t *event)
{
  lv_event_code_t code = lv_event_get_code(event);
  WristSettingsView_t *view = (WristSettingsView_t *)lv_event_get_user_data(event);
  lv_obj_t *target;
  uint8_t row;

  if ((code != LV_EVENT_CLICKED) || (view == NULL))
  {
    return;
  }

  target = lv_event_get_current_target(event);
  row = (uint8_t)(uintptr_t)lv_obj_get_user_data(target);
  if (row < WRIST_SETTINGS_ROW_COUNT)
  {
    if (view->selected == row)
    {
      WristSettings_ApplyRow(view, row);
    }
    else
    {
      view->selected = row;
      WristSettings_Refresh(view);
    }
  }
}

uint8_t WristSettings_IsOpen(const WristSettingsView_t *view)
{
  return ((view != NULL) && (view->overlay != NULL)) ? 1U : 0U;
}

void WristSettings_Open(WristSettingsView_t *view)
{
  static const char *labels[WRIST_SETTINGS_ROW_COUNT] = {
      WRIST_SETTINGS_TXT_HEART_KEY,
      WRIST_SETTINGS_TXT_VIBRATION,
      WRIST_SETTINGS_TXT_NIGHT,
      WRIST_SETTINGS_TXT_RESTORE
  };
  lv_obj_t *scr = lv_scr_act();
  lv_obj_t *title;
  uint8_t i;

  if ((view == NULL) || (view->overlay != NULL))
  {
    return;
  }

  view->selected = 0U;
  view->edit_heart_key_enabled = s_saved_heart_key_enabled;
  view->edit_vibration_enabled = s_saved_vibration_enabled;
  view->edit_night_mode = s_saved_night_mode;
  view->overlay = lv_obj_create(scr);
  lv_obj_remove_style_all(view->overlay);
  lv_obj_set_size(view->overlay, LV_PCT(100), LV_PCT(100));
  lv_obj_set_style_bg_color(view->overlay, (s_saved_night_mode != 0U) ? lv_color_hex(0x000000) : lv_color_hex(0x102B6B), 0);
  lv_obj_set_style_bg_opa(view->overlay, LV_OPA_40, 0);
  lv_obj_center(view->overlay);
  lv_obj_clear_flag(view->overlay, LV_OBJ_FLAG_SCROLLABLE);

  view->panel = lv_obj_create(view->overlay);
  lv_obj_remove_style_all(view->panel);
  lv_obj_set_size(view->panel, 214, 214);
  lv_obj_set_style_bg_color(view->panel, WristSettings_SurfaceColor(view), 0);
  lv_obj_set_style_bg_opa(view->panel, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(view->panel, 8, 0);
  lv_obj_set_style_border_width(view->panel, 1, 0);
  lv_obj_set_style_border_color(view->panel, WristSettings_BorderColor(view), 0);
  lv_obj_center(view->panel);
  lv_obj_clear_flag(view->panel, LV_OBJ_FLAG_SCROLLABLE);

  title = lv_label_create(view->panel);
  view->title = title;
  lv_obj_set_style_text_color(title, WristSettings_TextColor(view), 0);
  lv_obj_set_style_text_font(title, &wrist_font_zh_16, 0);
  lv_label_set_text(title, WRIST_SETTINGS_TXT_TITLE);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);

  for (i = 0U; i < WRIST_SETTINGS_ROW_COUNT; i++)
  {
    lv_obj_t *row = lv_obj_create(view->panel);
    lv_obj_t *label = lv_label_create(row);
    lv_obj_t *value = lv_label_create(row);

    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, 190, 33);
    lv_obj_set_style_radius(row, 6, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_border_color(row, lv_color_hex(0x1877F2), 0);
    lv_obj_set_style_border_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_align(row, LV_ALIGN_TOP_MID, 0, WRIST_SETTINGS_ROW_Y(i));
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_user_data(row, (void *)(uintptr_t)i);
    lv_obj_add_event_cb(row, WristSettings_RowEventCb, LV_EVENT_CLICKED, view);

    lv_obj_set_style_text_font(label, &wrist_font_zh_14, 0);
    lv_obj_set_style_text_color(label, WristSettings_TextColor(view), 0);
    lv_label_set_text(label, labels[i]);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 8, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_user_data(label, (void *)(uintptr_t)i);
    lv_obj_add_event_cb(label, WristSettings_RowEventCb, LV_EVENT_CLICKED, view);

    lv_obj_set_style_text_font(value, &wrist_font_zh_14, 0);
    lv_label_set_text(value, WristSettings_ValueText(view, i));
    lv_obj_align(value, LV_ALIGN_RIGHT_MID, -8, 0);
    lv_obj_add_flag(value, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_user_data(value, (void *)(uintptr_t)i);
    lv_obj_add_event_cb(value, WristSettings_RowEventCb, LV_EVENT_CLICKED, view);

    view->rows[i] = row;
    view->labels[i] = label;
    view->values[i] = value;
  }

  WristSettings_Refresh(view);
}

void WristSettings_Close(WristSettingsView_t *view)
{
  uint8_t i;

  if ((view == NULL) || (view->overlay == NULL))
  {
    return;
  }

  s_saved_heart_key_enabled = view->edit_heart_key_enabled;
  s_saved_vibration_enabled = view->edit_vibration_enabled;
  s_saved_night_mode = view->edit_night_mode;
  lv_obj_del(view->overlay);
  view->overlay = NULL;
  view->panel = NULL;
  view->title = NULL;
  for (i = 0U; i < WRIST_SETTINGS_ROW_COUNT; i++)
  {
    view->rows[i] = NULL;
    view->labels[i] = NULL;
    view->values[i] = NULL;
  }
}

void WristSettings_SelectNext(WristSettingsView_t *view)
{
  if ((view == NULL) || (view->overlay == NULL))
  {
    return;
  }

  view->selected = (uint8_t)((view->selected + 1U) % WRIST_SETTINGS_ROW_COUNT);
  WristSettings_Refresh(view);
}

void WristSettings_ApplySelected(WristSettingsView_t *view)
{
  if ((view == NULL) || (view->overlay == NULL))
  {
    return;
  }

  WristSettings_ApplyRow(view, view->selected);
}

uint8_t WristSettings_IsHeartKeyEnabled(void)
{
  return s_saved_heart_key_enabled;
}

uint8_t WristSettings_IsVibrationEnabled(void)
{
  return s_saved_vibration_enabled;
}

uint8_t WristSettings_IsNightMode(void)
{
  return s_saved_night_mode;
}
