/*
 * ct_main_win.h
 *
 * Copyright 2009-2021
 * Giuseppe Penone <giuspen@gmail.com>
 * Evgenii Gurianov <https://github.com/txe>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#pragma once

#include <unordered_map>
#include <optional>

#include <glibmm/i18n.h>
#include <gtkmm.h>
#include <gtksourceviewmm.h>
#include "ct_treestore.h"
#include "ct_misc_utils.h"
#include "ct_menu.h"
#include "ct_widgets.h"
#include "ct_config.h"
#include "ct_table.h"
#include "ct_image.h"
#include "ct_export2pdf.h"
#include "ct_state_machine.h"

struct CtStatusBar
{
    Gtk::Statusbar   statusBar;
    guint            statusId;
    Gtk::ProgressBar progressBar;
    Gtk::Button      stopButton;
    Gtk::Frame       frame;
    Gtk::HBox        hbox;

    void set_progress_stop(bool stop) { _progress_stop = stop; }
    bool is_progress_stop()           { return _progress_stop; }
    void push(const Glib::ustring& text) { statusBar.push(text, statusId); }
    void pop() { statusBar.pop(statusId); }
    void update_status(const Glib::ustring& text) { pop(); push(text); }

private:
    bool _progress_stop;
};

struct CtWinHeader
{
    Gtk::HBox        headerBox;
    Gtk::HButtonBox  buttonBox;
    Gtk::Label       nameLabel;
    Gtk::Image       lockIcon;
    Gtk::Image       bookmarkIcon;
    Gtk::EventBox    eventBox;
    std::map<Gtk::Button*, gint64> button_to_node_id;
};

class CtConfig;
class CtActions;
class CtTmp;
class CtMenu;
class CtPrint;
class CtStorageControl;

class CtMainWin : public Gtk::ApplicationWindow
{
public:
    CtMainWin(
        bool                     no_gui,
        CtConfig*                pCtConfig,
        CtTmp*                   pCtTmp,
        Gtk::IconTheme*          pGtkIconTheme,
        Glib::RefPtr<Gtk::TextTagTable> rGtkTextTagTable,
        Glib::RefPtr<Gtk::CssProvider> rGtkCssProvider,
        Gsv::LanguageManager*    pGsvLanguageManager,
        Gsv::StyleSchemeManager* pGsvStyleSchemeManager,
        CtStatusIcon*            pCtStatusIcon
    );
    virtual ~CtMainWin();

    void config_apply();
    void config_update_data_from_curr_status();
    void text_view_apply_cursor_position(CtTreeIter& treeIter, const int cursor_pos, const int v_adj_val);

    void update_theme();

    bool file_open(const fs::path& filepath, const std::string& node_to_focus, const Glib::ustring password = "");
    bool file_save_ask_user();
    void file_save(bool need_vacuum);
    void file_save_as(const std::string& new_filepath, const Glib::ustring& password);
    void file_autosave_restart();
    void mod_time_sentinel_restart();
    bool file_insert_plain_text(const fs::path& filepath);

    void reset();
    void update_window_save_needed(const CtSaveNeededUpdType update_type = CtSaveNeededUpdType::None,
                                   const bool new_machine_state = false,
                                   const CtTreeIter* give_tree_iter = nullptr);
    void load_buffer_from_state(std::shared_ptr<CtNodeState> state, CtTreeIter tree_iter);
    void re_load_current_buffer(const bool new_machine_state = false);
    void switch_buffer_text_source(Glib::RefPtr<Gsv::Buffer> text_buffer, CtTreeIter tree_iter, const std::string& new_syntax, const std::string& old_syntax);
    void update_window_save_not_needed();
    bool get_file_save_needed();

    void update_selected_node_statusbar_info();

    Glib::RefPtr<Gtk::TextBuffer>     curr_buffer() { return _ctTextview.get_buffer(); }
    CtTreeIter                        curr_tree_iter()  {
        return _uCtTreestore->to_ct_tree_iter(_uCtTreeview->get_selection()->get_selected());
    }
    CtTreeStore&                      get_tree_store()  { return *_uCtTreestore; }
    CtTreeView&                       get_tree_view()   { return *_uCtTreeview; }
    CtTextView&                       get_text_view()   { return _ctTextview; }
    CtStatusBar&                      get_status_bar()  { return _ctStatusBar; }
    CtMenu&                           get_ct_menu()     { return *_uCtMenu; }
    CtPrint&                          get_ct_print()    { return *_uCtPrint; }
    CtConfig*                         get_ct_config()   { return _pCtConfig; }
    CtStorageControl*                 get_ct_storage()  { return _uCtStorage.get(); }
    CtActions*                        get_ct_actions()  { return _uCtActions.get(); }
    CtTmp*                            get_ct_tmp()      { return _pCtTmp; }
    Gtk::IconTheme*                   get_icon_theme()  { return _pGtkIconTheme; }
    CtStateMachine&                   get_state_machine() { return _ctStateMachine; }
    Glib::RefPtr<Gtk::TextTagTable>&  get_text_tag_table() { return _rGtkTextTagTable; }
    Glib::RefPtr<Gtk::CssProvider>&   get_css_provider()   { return _rGtkCssProvider; }
    Gsv::LanguageManager*             get_language_manager() { return _pGsvLanguageManager; }
    Gsv::StyleSchemeManager*          get_style_scheme_manager() { return _pGsvStyleSchemeManager; }
    Gtk::StatusIcon*                  get_status_icon() { return _pCtStatusIcon->get(); }
    Gtk::ScrolledWindow&              getScrolledwindowText() { return _scrolledwindowText; }

    bool&         user_active()      { return _userActive; } // use as a function, because it's easier to put breakpoint
    bool&         force_exit()       { return _forceExit; }
    bool          no_gui()           { return _no_gui; }
    int&          cursor_key_press() { return _cursorKeyPress; }
    int&          hovering_link_iter_offset() { return _hovering_link_iter_offset; }

public:
    std::string               get_code_icon_name(std::string code_type);
    Gtk::Image*               new_image_from_stock(const std::string& stockImage, Gtk::BuiltinIconSize size);
    void                      apply_syntax_highlighting(Glib::RefPtr<Gsv::Buffer> text_buffer, const std::string& syntax, const bool forceReApply);
    void                      reapply_syntax_highlighting(const char target/*'r':RichText, 'p':PlainTextNCode, 't':Table*/);
    void                      resetup_for_syntax(const char target/*'r':RichText, 'p':PlainTextNCode*/);
    Glib::RefPtr<Gsv::Buffer> get_new_text_buffer(const Glib::ustring& textContent=""); // pygtk: buffer_create
    const std::string         get_text_tag_name_exist_or_create(const std::string& propertyName, const std::string& propertyValue);
    void                      apply_scalable_properties(Glib::RefPtr<Gtk::TextTag> rTextTag, CtScalableTag* pCtScalableTag);
    Glib::ustring             sourceview_hovering_link_get_tooltip(const Glib::ustring& link);
    bool                      apply_tag_try_automatic_bounds(Glib::RefPtr<Gtk::TextBuffer> text_buffer, Gtk::TextIter iter_start);
    void                      apply_tag_try_automatic_bounds_triple_click(Glib::RefPtr<Gtk::TextBuffer> text_buffer, Gtk::TextIter iter_start);

private:
    Gtk::HBox&     _init_status_bar();
    Gtk::EventBox& _init_window_header();

public:
    void window_title_update(std::optional<bool> saveNeeded = std::nullopt);

    void window_header_update();
    void window_header_update_lock_icon(bool show);
    void window_header_update_bookmark_icon(bool show);

    void menu_update_bookmark_menu_item(bool is_bookmarked);
    void menu_set_bookmark_menu_items();
    void menu_top_optional_bookmarks_enforce();

    void menu_set_items_recent_documents();
    void menu_set_visible_exit_app(bool visible);
    void menu_rebuild_toolbars(bool new_toolbar);

    void config_switch_tree_side();

    void show_hide_toolbars(bool visible)   { for (auto pToolbar: _pToolbars) pToolbar->property_visible() = visible; }
    void set_toolbars_icon_size(int size)   { for (auto pToolbar: _pToolbars) pToolbar->property_icon_size() = CtMiscUtil::getIconSize(size); }
    void show_hide_tree_view(bool visible)  { _scrolledwindowTree.property_visible() = visible; }
    void show_hide_win_header(bool visible) { _ctWinHeader.headerBox.property_visible() = visible; }

    void resetPrevTreeIter()                { _prevTreeIter = CtTreeIter(); }

    void save_position()                    { get_position(_savedXpos, _savedYpos); }
    void restore_position()                 { if (_savedXpos != -1) move(_savedXpos, _savedYpos); }

private:
    bool _on_window_key_press_event(GdkEventKey* event);
    bool _on_window_configure_event(GdkEventConfigure* configure_event);

    void _on_treeview_cursor_changed(); // pygtk: on_node_changed
    bool _on_treeview_button_release_event(GdkEventButton* event);
    void _on_treeview_event_after(GdkEvent* event); // pygtk: on_event_after_tree
    void _on_treeview_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);
    bool _on_treeview_test_collapse_row(const Gtk::TreeModel::iterator&,const Gtk::TreeModel::Path&);
    bool _on_treeview_key_press_event(GdkEventKey* event);
    bool _on_treeview_popup_menu();
    bool _on_treeview_scroll_event(GdkEventScroll* event);
    bool _on_treeview_drag_motion(const Glib::RefPtr<Gdk::DragContext>& context,
                                                 int x,
                                                 int y,
                                                 guint time);
    void _on_treeview_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context,
                                         int x,
                                         int y,
                                         const Gtk::SelectionData& selection_data,
                                         guint info,
                                         guint time);
    void _on_treeview_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context,
                                    Gtk::SelectionData& selection_data,
                                    guint info,
                                    guint time);

    void _on_textview_populate_popup(Gtk::Menu* menu);
    bool _on_textview_motion_notify_event(GdkEventMotion* event);
    bool _on_textview_visibility_notify_event(GdkEventVisibility* event);
    void _on_textview_size_allocate(Gtk::Allocation& allocation);
    bool _on_textview_event(GdkEvent* event); // pygtk: on_sourceview_event
    void _on_textview_event_after(GdkEvent* event); // pygtk: on_sourceview_event_after
    bool _on_textview_scroll_event(GdkEventScroll* event);

    void _reset_CtTreestore_CtTreeview();
    void _ensure_curr_doc_in_recent_docs();
    void _zoom_tree(bool is_increase);
    bool _try_move_focus_to_anchored_widget_if_on_it();

private:
    const bool                   _no_gui;
    CtConfig*                    _pCtConfig;
    CtTmp*                       _pCtTmp;
    Gtk::IconTheme*              _pGtkIconTheme;
    Glib::RefPtr<Gtk::TextTagTable> _rGtkTextTagTable;
    Glib::RefPtr<Gtk::CssProvider>  _rGtkCssProvider;
    Gsv::LanguageManager*        _pGsvLanguageManager;
    Gsv::StyleSchemeManager*     _pGsvStyleSchemeManager;
    CtStatusIcon*                _pCtStatusIcon;

    std::unique_ptr<CtActions>        _uCtActions;
    std::unique_ptr<CtMenu>           _uCtMenu;
    std::unique_ptr<CtPrint>          _uCtPrint;
    std::unique_ptr<CtStorageControl> _uCtStorage;

    Gtk::VBox                    _vboxMain;
    Gtk::VBox                    _vboxText;
    Gtk::HPaned                  _hPaned;
    Gtk::MenuBar*                _pMenuBar{nullptr};
    std::vector<Gtk::Toolbar*>   _pToolbars;
    CtStatusBar                  _ctStatusBar;
    CtWinHeader                  _ctWinHeader;
    std::array<Gtk::MenuItem*,3> _pBookmarksSubmenus{nullptr,nullptr,nullptr};
    Gtk::MenuItem*               _pRecentDocsSubmenu{nullptr};
    Gtk::MenuToolButton*         _pRecentDocsMenuToolButton{nullptr};
    Gtk::ScrolledWindow          _scrolledwindowTree;
    Gtk::ScrolledWindow          _scrolledwindowText;
    std::unique_ptr<CtTreeStore> _uCtTreestore;
    std::unique_ptr<CtTreeView>  _uCtTreeview;
    CtTextView                   _ctTextview;
    CtStateMachine               _ctStateMachine;
    std::unique_ptr<CtPairCodeboxMainWin> _uCtPairCodeboxMainWin;

    Glib::RefPtr<Gtk::CssProvider> _css_provider_theme;

private:
    bool                _userActive{true}; // pygtk: user_active
    bool                _forceExit{false};
    int                 _cursorKeyPress{-1};
    int                 _hovering_link_iter_offset{-1};
    int                 _prevTextviewWidth{0};
    bool                _fileSaveNeeded{false}; // pygtk: file_update
    std::unordered_map<gint64, gint64> _latestStatusbarUpdateTime; // pygtk: latest_statusbar_update_time
    CtTreeIter          _prevTreeIter;
    int                 _savedXpos{-1};
    int                 _savedYpos{-1};
    sigc::connection    _autosave_timout_connection;
    sigc::connection    _mod_time_sentinel_timout_connection;
    bool                _tree_just_auto_expanded{false};
    std::unordered_map<gint64, int> _nodesCursorPos;
    std::unordered_map<gint64, int> _nodesVScrollPos;

public:
    sigc::signal<void>             signal_app_new_instance = sigc::signal<void>();
    sigc::signal<void>             signal_show_hide_main_win = sigc::signal<void>();
    sigc::signal<void, std::function<void(CtMainWin*)>>
                                   signal_app_apply_for_each_window = sigc::signal<void, std::function<void(CtMainWin*)>>();

    sigc::signal<void, CtMainWin*> signal_app_quit_or_hide_window = sigc::signal<void, CtMainWin*>();
    sigc::signal<void, CtMainWin*> signal_app_quit_window = sigc::signal<void, CtMainWin*>();
};
