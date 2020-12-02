/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/sidebar/sidebar_service.h"

#include "base/feature_list.h"
#include "base/strings/utf_string_conversions.h"
#include "base/values.h"
#include "brave/components/sidebar/features.h"
#include "brave/components/sidebar/pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/scoped_user_pref_update.h"

namespace sidebar {

namespace {

constexpr char kSidebarItemURLKey[] = "url";
constexpr char kSidebarItemTypeKey[] = "type";
constexpr char kSidebarItemTitleKey[] = "title";
constexpr char kSidebarItemOpenInPanelKey[] = "open_in_panel";

std::vector<SidebarItem> GetDefaultSidebarItems() {
  std::vector<SidebarItem> items;
  items.push_back(SidebarItem::Create(GURL("https://together.brave.com/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      true));
  items.push_back(SidebarItem::Create(GURL("brave://wallet/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      false));
  items.push_back(SidebarItem::Create(GURL("brave://bookmarks/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      true));
  items.push_back(SidebarItem::Create(GURL("brave://history/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      true));
    return items;
}

}  // namespace

// static void
void SidebarService::RegisterPrefs(PrefRegistrySimple* registry) {
  if (!base::FeatureList::IsEnabled(kSidebarFeature))
    return;

  registry->RegisterListPref(kSidebarItems);
}

SidebarService::SidebarService(PrefService* prefs) : prefs_(prefs) {
  DCHECK(prefs_);
  LoadSidebarItems();
}

SidebarService::~SidebarService() = default;

void SidebarService::AddItem(const SidebarItem& item) {
  items_.push_back(item);

  for (Observer& obs : observers_) {
    // Index starts at zero.
    obs.OnItemAdded(item, items_.size() - 1);
  }

  UpdateSidebarItemsToPrefStore();
}

void SidebarService::RemoveItem(const SidebarItem& item) {
}

void SidebarService::UpdateSidebarItemsToPrefStore() {
  ListPrefUpdate update(prefs_, kSidebarItems);
  update->ClearList();

  for (const auto& item : items_) {
    base::Value dict(base::Value::Type::DICTIONARY);
    dict.SetStringKey(kSidebarItemURLKey, item.url.spec());
    dict.SetStringKey(kSidebarItemTitleKey, base::UTF16ToUTF8(item.title));
    dict.SetIntKey(kSidebarItemTypeKey, static_cast<int>(item.type));
    dict.SetBoolKey(kSidebarItemOpenInPanelKey, item.open_in_panel);
    update->Append(std::move(dict));
  }
}

void SidebarService::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void SidebarService::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void SidebarService::LoadSidebarItems() {
  const auto& items = prefs_->Get(kSidebarItems)->GetList();
  if (items.empty()) {
    items_ = GetDefaultSidebarItems();
    return;
  }

  for (const auto& item : items) {
    // Only add |item| if it has all properties.
    std::string url;
    if (const auto* value = item.FindStringKey(kSidebarItemURLKey)) {
      url = *value;
    } else {
      continue;
    }

    SidebarItem::Type type;
    if (const auto value = item.FindIntKey(kSidebarItemTypeKey)) {
      type = static_cast<SidebarItem::Type>(*value);
    } else {
      continue;
    }

    std::string title;
    if (const auto* value = item.FindStringKey(kSidebarItemTitleKey)) {
      title = *value;
    } else {
      continue;
    }

    bool open_in_panel;
    if (const auto value = item.FindBoolKey(kSidebarItemOpenInPanelKey)) {
      open_in_panel = *value;
    } else {
      continue;
    }

    items_.push_back(SidebarItem::Create(GURL(url),
                                         base::UTF8ToUTF16(title),
                                         type,
                                         open_in_panel));
  }
}

}  // namespace sidebar
