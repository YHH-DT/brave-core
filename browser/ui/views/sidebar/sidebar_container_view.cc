/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <memory>

#include "brave/browser/ui/views/sidebar/sidebar_container_view.h"

#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/sidebar/sidebar_controller.h"
#include "brave/browser/ui/sidebar/sidebar_model.h"
#include "brave/browser/ui/sidebar/sidebar_model_data.h"
#include "brave/browser/ui/views/sidebar/sidebar_control_view.h"
#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/browser_context.h"
#include "ui/views/border.h"
#include "ui/views/controls/webview/webview.h"
#include "url/gurl.h"

SidebarContainerView::SidebarContainerView(BraveBrowser* browser)
    : browser_(browser) {
}

SidebarContainerView::~SidebarContainerView() = default;

void SidebarContainerView::Init() {
  initialized_ = true;

  sidebar_model_ = browser_->sidebar_controller()->model();
  observed_.Add(sidebar_model_);

  // TODO(simonhong): Get color via theme properties.
  // We can handle dark/light theme color with theme properties.
  constexpr SkColor kBorderColor = SkColorSetRGB(0xD9, 0xDC, 0xDF);
  constexpr int kBorderThickness = 1;
  SetBorder(
      views::CreateSolidSidedBorder(0, 0, 0, kBorderThickness, kBorderColor));

  AddChildViews();
}

void SidebarContainerView::AddChildViews() {
  sidebar_control_view_ =
      AddChildView(std::make_unique<SidebarControlView>(browser_));
  sidebar_panel_view_ = AddChildView(
      std::make_unique<views::WebView>(browser_->profile()));
  // |sidebar_panel_view_| will be visible when user opens sidebar panel.
  sidebar_panel_view_->SetVisible(false);
}

void SidebarContainerView::Layout() {
  // TODO(simonhong): Use layout manager.
  if (!initialized_)
    return View::Layout();

  const int control_view_preferred_width =
      sidebar_control_view_->GetPreferredSize().width();
  // TODO(simonhong): Handle sidebar left/right position here.
  sidebar_control_view_->SetBounds(
      0, 0,
      control_view_preferred_width, height());
  if(sidebar_panel_view_->GetVisible()) {
    sidebar_panel_view_->SetBounds(
        control_view_preferred_width, 0,
        width() - control_view_preferred_width - GetInsets().width(), height());
  }
}

gfx::Size SidebarContainerView::CalculatePreferredSize() const {
  if (!initialized_)
    return View::CalculatePreferredSize();

  const int control_view_preferred_width =
      sidebar_control_view_->GetPreferredSize().width();

  constexpr int kSidebarContainerViewFullWidth = 380;
  return {sidebar_panel_view_->GetVisible() ? kSidebarContainerViewFullWidth
                                            : control_view_preferred_width,
          0};
}

void SidebarContainerView::OnActiveIndexChanged(int old_index, int new_index) {
  if (new_index == -1) {
    sidebar_panel_view_->SetVisible(false);
  } else {
    auto* item_data = sidebar_model_->data()[new_index].get();
    if (item_data->OpenInPanel()) {
      auto* web_contents = item_data->GetWebContents();
      sidebar_panel_view_->SetWebContents(web_contents);
      // TODO(simonhong): Maybe we need reloading.
      if (!item_data->loaded()) {
        item_data->set_loaded(true);
        sidebar_panel_view_->LoadInitialURL(item_data->GetURL());
      }
      sidebar_panel_view_->SetVisible(true);
    } else {
      sidebar_panel_view_->SetVisible(false);
    }
  }

  InvalidateLayout();
  Layout();
}
