Glitch is a graphical lightweight interactive tool and compiler hybrid.
Generate Arduino software via beautiful graphical diagrams (blueprints).
Now with a natural language processor!
Glitch: connect (a, b) (c, d).
Glitch: new int variable (a, b).
Glitch: select left-most objects; align.

Version 2024.09.25 is the newest LTS release.<br>
Version 2024.11.25 is the newest source release.

<h3><b>Summary of Glitch</b></h3>

<li>ARM, PowerPC, Sparc, etc.</li>
<li>ASH, or, a natural language shell.</li>
<li>Android, FreeBSD, Linux, MacOS, Windows, etc.</li>
<li>Android-friendly design.</li>
<li>Apple Silicon!</li>
<li>Arduino!</li>
<li>Available almost everywhere!</li>
<li>Configurable widgets.</li>
<li>Copy-and-paste.</li>
<li>Diagrams are recorded in portable SQLite databases.</li>
<li>Drag-and-drop.</li>
<li>Extensible with other frameworks.</li>
<li><b>G</b>raphical <b>l</b>ightweight <b>i</b>nteractive <b>t</b>ool and
    <b>c</b>ompiler <b>h</b>ybrid.</li>
<li>Infinite diagrams.</li>
<li>Mindful drawing.</li>
<li>Modern C++.</li>
<li>Qt 5.5.1 (PowerPC)!</li>
<li>Qt LTS.</li>
<li>Qt-only dependency.</li>
<li>Raspberry Pi!</li>
<li>Rich redo / undo stack.</li>
<li>Serial port reader and writer.</li>
<li>Tabbed edit windows!</li>

<h3><b>Release Notes</b></h3>

<p>2024.11.25</p>
<ul>
<li>Edit window title shall now include the canvas name.</li>
<li>Eliminated tool bar flicker. So many wonderful details.</li>
<li>Include scaling information in the status bar of an edit
  window.</li>
<li>Maximize Edit Windows canvas setting.</li>
<li>Removed Doxygen.</li>
<li>Tabbed edit windows. Enabled. New canvas setting.</li>
</ul>
<p>2024.11.12</p>
<ul>
<li>Arduino: upload and verify functions.</li>
<li>Generate output file before launching the IDE.</li>
<li>Include scaling information in the status bar.</li>
<li>New LTS version string information for About.</li>
<li>Object order was incorrect upon loading of diagram because
  of invalid object position.</li>
<li>Print.</li>
</ul>
<p>2024.09.25</p>
<ul>
<li>ASH: copy, paste commands.</li>
<li>Allow for canvas color alphas: background color,
  dots / grids color, lock color, selection color, source text color,
  wire color.</li>
<li>Arrow object and background color alpha; in addition to
  transparency property!</li>
<li>Awesome button colors.</li>
<li>Bold ASH text.</li>
<li>Corrected glitch.pro. Deleting debug plugins may delete
  required plugins files in the Windows install target. Qt 6.5 LTS
  has separated the debug and release files and the additional logic in
  glitch.pro is not necessary.</li>
<li>Ctrl + W and docked property editors. Duplicating shortcuts
  causes problems.</li>
<li>Default Arduino IDE of /usr/bin/arduino on Linux.</li>
<li>Display icons of dragged objects.</li>
<li>Fewer changed() signals during the saving of a diagram.</li>
<li>LTS release!</li>
<li>Many constants.</li>
<li>Object background color alpha.</li>
<li>Pasting into a canvas preview is now natural. Previously,
  a paste event's coordinates would be translated.</li>
<li>Pasting into a view will not require a mouse click event.</li>
<li>Save periodically. New Edit -> Canvas Settings -> Project ->
  Save Periodically.</li>
<li>The Windows bundle includes some additional revisions.
Please read
<a href = "https://github.com/textbrowser/glitch/compare/2024.09.25...master">
https://github.com/textbrowser/glitch/compare/2024.09.25...master</a>.</li>
<li>Twenty years of Glitch!</li>
</ul>
<p>2024.07.30</p>
<ul>
<li>Allow categories to exist in their own floating dialogs. Dialogs
  are available via context menus.</li>
<li>Constant variables and paint events.</li>
<li>Drag-and-drop: display an item's icon if the item is ready for a
  drag.</li>
</ul>
<p>2024.06.05</p>
<ul>
<li>Corrected dragging and dropping of multiple items. The
  current implementation creates multiple copies of a single
  item.</li>
<li>Dragging a category onto a canvas will cause an abnormal
  termination because the category's parent() object does not
  exist because a category is a top-level item.</li>
<li>First Apple Silicon release.</li>
<li>LTS release!</li>
<li>Redo / undo history panel.</li>
<li>Serial port in the PowerPC distribution.</li>
<li>Source Preview Close icon.</li>
</ul>
<p>2024.04.30</p>
<ul>
<li>ASH behavior: list local objects if a list argument is not
  provided.</li>
<li>ASH: list local must return local objects only.</li>
<li>ASH: list-statistics.</li>
<li>ASH: sort list results.</li>
<li>Added Ctrl+W shortcut to User Functions's Close button.</li>
<li>Avoid noisy signals between Find Objects and modified
  objects.</li>
<li>Avoid verbose signals after committing canvas settings.</li>
<li>Center child window with respect to separated diagram if
  necessary.</li>
<li>Copy selected ASH text via Control + C.</li>
<li>Destroy magic proxies in glitch_scene::purgeRedoUndoProxies().
  Such local proxies exist within the universal redo / undo stack
  and must be purged when their parent scenes are destroyed.</li>
<li>Display the number of objects in Find Objects.</li>
<li>Do not generate a canvas's source view unless a
  Source View window is visible.</li>
<li>Floating source preview for child items.</li>
<li>Inspect scene() results. Curious mishaps.</li>
<li>Interrupt an ASH prompt via Control + Shift + C instead of
  Control + C.</li>
<li>New --generate-source terminal option.</li>
<li>New File -> Close All Diagrams.</li>
<li>New default value of false: Canvas Settings -> Source View ->
  Generate Source View Periodically.</li>
<li>Page scrolling in ASH.</li>
<li>QString::split() and skip-empty-parts.</li>
<li>Remove leading and trailing space characters from ASH
  commands.</li>
<li>Removed all iterators from recent-diagrams highlight logic.</li>
<li>Renamed database table properties to diagram_properties.</li>
<li>Separated diagrams and source previews.</li>
<li>Speedier object instantiation.</li>
<li>The option --open-arduino-diagram does not require absolute
  file names.</li>
<li>The option --open-arduino-diagram is now
  --open-arduino-diagrams.</li>
</ul>
<p>2024.03.25</p>
<ul>
<li>Block signals in glitch_misc::sortCombinationBox() before
  altering the provided combination box widget.</li>
<li>Floating source preview. Please see Windows. Click an object
  and read its source!</li>
<li>Improved text search in documentation class.</li>
<li>Inspect redo / undo stack objects and provide
  alternate logic if necessary.</li>
<li>New Arduino keyword: //.</li>
<li>New default canvas background color #55aaff.</li>
<li>Write comments on multiple lines. Modifies an item from the
  2023.09.10 release.</li>
</ul>
<p>2024.02.29</p>
<ul>
<li>A source-only review for a planet of lunatics.</li>
<li>ASH of child views.</li>
<li>ASH: !history-item (repeat),
  clear-history,
  delete,
  history,
  list all,
  list local,
  save-history-desktop.</li>
<li>ASH: support the Home key.</li>
<li>Drag and copy!</li>
<li>Editable size in widget context menu.</li>
<li>This release is dedicated to the glitchy Mr. D. Peace out!</li>
</ul>
<p>2024.01.15</p>
<ul>
<li>ASH, design via a human-like language. An introduction.</li>
<li>Connect close shortcuts to close buttons.</li>
<li>Corrected Debian control files. The files do not contain
  the required dependencies. GitHub ticket #9.</li>
<li>Find Objects is now a dialog instead of a window.</li>
<li>Highlight Recent Diagrams item if selected via the keyboard.</li>
<li>Icons: zoom-reset on canvas context menu.</li>
<li>Include object identifier in Find Objects.</li>
<li>LTS release!</li>
<li>New --full-screen.</li>
<li>New errors covering Examples permissions. GitHub ticket #10.</li>
<li>QString::split() and skip-empty-parts.</li>
<li>Qt 6: size_t qHash() instead of uint qHash().</li>
<li>Show image of dragged item during creation.</li>
<li>The option --help did not include --show-serial-port-window.</li>
</ul>
<p>2023.11.20</p>
<ul>
<li>Center edit windows with respect to the main window.</li>
<li>Improved resizing of syntax object.</li>
<li>New Arduino constants and conversions.</li>
<li>Zoom Reset tool.</li>
</ul>
<p>2023.10.30</p>
<ul>
<li>Adjust-size created shadows. Corrected!</li>
<li>Added Other to Syntax category.</li>
<li>Allow assignment of value during initialization of array variable.</li>
<li>Beautiful zoom tool bar.</li>
<li>Comment-like object property: Generate Source. Objects will appear
  transparent.</li>
<li>Corrected spelling: macros() -> micros().</li>
<li>Disable some keyboard events on read-only text-input fields.</li>
<li>Improved recording of splitter states.</li>
<li>Initialize the Tabs menu on MacOS.</li>
<li>LTS release! See the bundles of love.</li>
<li>New Arduino type: uint32_t.</li>
<li>New Catalog category.</li>
<li>New auto keyword as variable type.</li>
<li>New input dialog sizes.</li>
<li>New tool bar titles. Mostly invisible, however, present.</li>
<li>Qt 6.5.3 LTS Android.</li>
<li>Qt 6.5.3 LTS MacOS.</li>
<li>Qt 6.5.3 LTS Windows.</li>
<li>Removed bounces which are observable during resizing of compact
  widgets.</li>
<li>Save edit window states on close events.</li>
<li>Synchronize diagram name with Tabs menu.</li>
<li>The size property of a widget must be restored in a delayed
  manner as some properties are prepared in derived classes.</li>
</ul>
<p>2023.09.25</p>
<ul>
<li>Bouncy-less object resizing.</li>
<li>New Serial Port Window layout.</li>
<li>New show-serial-port-window terminal option.</li>
<li>Object resize optimizations: object caches, switch statements.</li>
<li>Prepared the project file for FreeBSD and Qt 6.</li>
<li>Removed duplicate instructions in widget resizing.</li>
<li>Retain Serial Port Window geometry and layout.</li>
<li>Source-only release. Adopting an LTS-release format.</li>
<li>Variable spelling: m_rectanges -> m_rectangles.</li>
<li>Write serial port data as UTF-8 instead of Latin.</li>
</ul>
<p>2023.09.10</p>
<ul>
<li>Added QPainter::LosslessImageRendering where supported. Requires
  Qt 5.13 and newer.</li>
<li>Added context menu item to variable object for setting the
  variable's type.</li>
<li>Added default keyword to Arduino flow control object.</li>
<li>Clearer rendering of diagram previews.</li>
<li>Destroy a tab's save push-button whenever its diagram is
  destroyed. Otherwise, resources will not be released.</li>
<li>Disable WA_AcceptTouchEvents on MacOS. Resolves
  qt.pointer.dispatch: skipping QEventPoint() warning.</li>
<li>Display a diagram's tab before loading it.</li>
<li>Display only non-array types in Variables category.</li>
<li>Do not register a redo / undo if a widget is moved to its current
  position.</li>
<li>Emit editingFinished() in glitch_object_lineedit::leaveEvent().</li>
<li>Highlight problematic Recent Diagrams entries on non-Android and
  non-MacOS.</li>
<li>Improved processing of restoring diagrams from database containers.
  PowerPC and Windows only. Other systems will receive this item in a
  future release.</li>
<li>Increased generate-source-view timer interval to 1.5 seconds.</li>
<li>Initialize the output_directory INI setting to the operator's
  home directory. If the setting is not initialized, a source file
  cannot be generated.</li>
<li>New lock and selection colors.</li>
<li>New minimum height for comment object.</li>
<li>New sizeHint() implementation for flow-control object.</li>
<li>Partially completed SipHash example. Cryptography in Arduino
  through Glitch!</li>
<li>Resize edit windows to 85% of the main window's size. New
  windows only.</li>
<li>Serial Port Window. Please see Windows menu.</li>
<li>Simplified selection border of selected objects.</li>
<li>Write comments on single lines.</li>
</ul>
<p>2023.08.19</p>
<ul>
<li>Corrected abnormal termination caused by redo / undo delete of
  nested functions.</li>
<li>Corrected move and resize cursors.</li>
<li>Corrected redo / undo of function return type.</li>
<li>Create actions for special tools. If the actions are not created,
  special functions such as group locking will fail.</li>
<li>Disable diagram separation on Android.</li>
<li>Double-clicking on an empty portion of a canvas will
  display the diagram's context menu.</li>
<li>Hide a widget's resize helpers whenever the widget is dragged.</li>
<li>Improved mouse interaction in proximity of wires.</li>
<li>Improved positioning of context menus. A menu's top-left-corner
  position should be the cursor's position.</li>
<li>Interactive scene previews. Double-clicking on a scene will place
  the major theme into view.</li>
<li>New Glitch preference: Override Widget Fonts.</li>
<li>Nicer Recent Diagrams for non-Android and non-MacOS.</li>
<li>Open function edit windows via double-click on docked function
  views.</li>
<li>Set a minimum width when restoring a widget from its
  database.</li>
<li>Set icon sizes on separated window tool bars.</li>
<li>Show context menu on widget double-click event.</li>
<li>Simplified block comment widget.</li>
<li>Support returning pointers in functions.</li>
<li>Zoom feature: View -> Zoom. See Windows -> Glitch Preferences.</li>
</ul>
<p>2023.07.30</p>
<ul>
<li>Android: new close buttons, properly close windows
  (hide() instead of close()), showMaximized() instead of showNormal(),
  tools menus (menus are shown in separate windows).</li>
<li>Attached descriptive tool tips to objects.</li>
<li>Darkmode for Windows.</li>
<li>Double-clicking a function clone will now launch the function's
  edit window.</li>
<li>Edit menu items are now present in the tool bar.</li>
<li>File menu items are now present in the main window's tool bar.</li>
<li>Font tool will now cover all widgets of a diagram.</li>
<li>Generate Source View Periodically is now enabled for new diagrams.</li>
<li>Intelligent timers preserve resources.</li>
<li>Linux-like distributions: /usr/local -> /opt.</li>
<li>New Arduino variable types: BitOrder, PinStatus, pin_size_t,
  uint8_t, uint16_t.</li>
<li>New minimum-size-hint methods.</li>
<li>New tool: Adjust Size(s) (All Widgets).</li>
<li>New tool: Widget(s) Properties (Selected Widget(s)).</li>
<li>New widget sizes. Please resize existing widgets!</li>
<li>Properly sort User Functions whenever a function is added or
  renamed.</li>
<li>Qt 6.5.2 LTS for Android.</li>
<li>Qt 6.5.2 LTS for Windows.</li>
<li>Removed Stream object as this is an abstract, base class.</li>
<li>Saving a diagram as PNG image(s).</li>
<li>Scoped C++ enumerators.</li>
<li>Special method for installing Examples on Android.</li>
<li>User Functions are now docked in edit windows.</li>
<li>Widgets now ignore the application-level font. Widgets provide
  custom fonts.</li>
</ul>
<p>2023.06.30</p>
<ul>
<li>A widget may be deleted in glitch_scene::artificialDrop() because
  of an addObject() failure. Improved the logic of artificialDrop().</li>
<li>Added Generate -> Source View to separated canvases.</li>
<li>Added Tools to tool bars.</li>
<li>Added analogReadResolution().</li>
<li>Adjust the size of a widget after it's been compressed or
  decompressed.</li>
<li>All distributions now include the Examples directory.</li>
<li>Allow selecting of text in widgets housing text fields.</li>
<li>Arrow object may or may not be transparent; corrected action
  availability.</li>
<li>Attempt to prepare a widget's edit window's geometry.</li>
<li>Center an edit window if restoreGeometry() fails.</li>
<li>Copying wired function clones is now complete!</li>
<li>Corrected MacOS Qt warning: outside any known screen, using
  primary screen.</li>
<li>Corrected adding of compound operator -= via drag & drop.</li>
<li>Corrected derelict 'else' in glitch_object_variable_arduino::
  setProperties().</li>
<li>Corrected drawing of arrow object. Large arrows are distorted.</li>
<li>Corrected state of Clear Copied Widgets Buffer in the main window.</li>
<li>Corrected state of Paste function in canvas floating menu.</li>
<li>Corrected widget transformations. Function types were incomplete.</li>
<li>Decreased the number of instructions in glitch_object():
  retrieve a new identifier for only a new widget.</li>
<li>Default Arduino syntax colors for new diagrams.</li>
<li>Denote widget type in context menu.</li>
<li>Display a constant's value as its name.</li>
<li>Display the Canvas Settings dialog if the IDE is
  not accessible after a Show Project IDE request.</li>
<li>Do not draw dots and grids if their colors match the
  brush color of their parent canvas. Fewer instructions!</li>
<li>Double-click events will open context menus or edit windows.</li>
<li>Emit changed() signal in glitch_object::slotPropertyChanged(),
  glitch_object::slotSelectColor() if necessary.</li>
<li>File Tool Bar now precedes the Tool Bar. New Glitch instances.</li>
<li>Find Objects now includes a search field.</li>
<li>Functions added via User Functions are mislabeled (toLower()).
  Corrected!</li>
<li>Generate source and copy it into the clipboard buffer.</li>
<li>Glitch will be shown in a maximized window for flawed or new
  instances.</li>
<li>Group operations will now create grouped redo / undo commands.</li>
<li>Highlight syntax widget text.</li>
<li>Improved detection of invalid variable names.</li>
<li>Initialize Docked Widget Properties Editor's size.</li>
<li>Native icons.</li>
<li>New Canvas Settings option: Source View -> Generate Source
  View Periodically.</li>
<li>New Edit -> Separate Current Canvas.</li>
<li>New Find Objects column: Type Total.</li>
<li>New default redo / undo stack size of 1500.</li>
<li>New icons added to several widgets.</li>
<li>New layout in Glitch Preferences.</li>
<li>New tool: (Un)lock Position(s) (Selected Widget(s)).</li>
<li>New tool: Disconnect Selected Widget(s).</li>
<li>New tool: Fonts (All Widgets).</li>
<li>Non-empty Arduino variable names.</li>
<li>Qt 5.15.10 LTS for MacOS.</li>
<li>Qt 6.5.1 LTS for Android.</li>
<li>Qt 6.5.1 LTS for Windows. Windows 7 has been removed.</li>
<li>Raise newly-separated diagrams.</li>
<li>Record absolute file paths for recent files.</li>
<li>Reinitialize Glitch Preferences on show event. That is,
  discard revisions which have not been committed.</li>
<li>Removed --show-arduino-structures as it's deprecated.</li>
<li>Removed Qt::WindowStaysOnTopHint.</li>
<li>Removed legacy SQLite statements.</li>
<li>Removed virtual methods from constructors.</li>
<li>Render property colors in floating context menus through
  manufactured icons!</li>
<li>Repaint a wire whenever its parent scene is updated.</li>
<li>Replaced QLabel Object Name widget with QLineEdit in
  Floating Context Menu panel.</li>
<li>Resize widgets upon drop events.</li>
<li>Retain Find Objects selection after a synchronization event.</li>
<li>Retain Tools mode before the Tools window is displayed.</li>
<li>Saving newly-created diagrams was incomplete because of
  invalid object identifiers. For example, a widget attached to
  an editable widget would not be saved.</li>
<li>Set function return type via context menu.</li>
<li>Simplified block comments: /*\nText\n*/.</li>
<li>Synchronize the contents of Find Objects if the synchronization
  widget is activated.</li>
<li>The Tools value does not retain the proper setting after a
  separated canvas is united.</li>
<li>Widget font.</li>
<li>Widget input fields have been replaced with special widgets
  so that double-click events are now required to interact
  with the field text. The new widgets provide more natural widget
  manipulation.</li>
<li>Windows x86-64 only.</li>
<li>Wired objects may now be disconnected via wire clicks.</li>
<li>loop and setup are Arduino keywords.</li>
</ul>
<p>2023.05.20</p>
<ul>
<li>Activate minimized windows if necessary.</li>
<li>An object's context menu is now accessible via Find Objects.
  Simply ignite a context menu over the desired object in Find Objects.</li>
<li>Arduino IDE is optional. Do not prevent file generation.</li>
<li>Corrected color sorting in Canvas Settings -> Source View.</li>
<li>Corrected tear-off-menus default INI value.</li>
<li>If necessary, highlight required variable name on creation.</li>
<li>Multiple objects may now be dragged onto a diagram.</li>
<li>Natively-drawn widgets may now be optionally transparent.</li>
<li>New tools: Adjust Size, Compress.</li>
<li>Object transformation. For example, noTone() -> tone()
  via context menu.</li>
<li>Portable output files. New Glitch Preferences setting.</li>
<li>PowerPC Debian release!</li>
<li>Removed Other, block comment, and function() from Arduino keywords.</li>
<li>Removed redundant sorting from Canvas Settings -> Source View.</li>
<li>Scroll a view if an object is moved via keyboard arrow keys.</li>
</ul>
<p>2023.04.30</p>
<ul>
<li>Added New Diagram -> Arduino tool bar action.</li>
<li>Background color property for natively-drawn widget.</li>
<li>Corrected default value of setting
  preferences/docked_widget_property_editors.</li>
<li>Corrected pointer comparison in
  glitch_canvas_settings::slotSelectColor().</li>
<li>Dock property editors of children of editable objects.</li>
<li>Edit tool bar.</li>
<li>Enable native dialogs and menus. File dialogs are excluded.</li>
<li>Font color property for natively-drawn widget.</li>
<li>Lock color. Please see Canvas Settings -> Canvas.</li>
<li>Manually resize objects!</li>
<li>Native drawing of many Arduino structures.</li>
<li>Pi-64 (aarch64) for Debian. A package will not be created.</li>
<li>Properties tool tips.</li>
<li>Q_OS_MAC is deprecated. Replaced with Q_OS_MACOS.</li>
<li>Qt 6.5 LTS compliant!</li>
<li>Replaced color property with background color for arrow object.</li>
<li>Retain an edit window's geometry and state upon save event.</li>
<li>Save state of a separated canvas is not reflected in the window's
  title.</li>
<li>Simplified Recent Diagrams menu; native menus.</li>
<li>Smoother preparation of tool bars through delayed painting.</li>
<li>Vacuum glitch_recent_files.db after processing.</li>
</ul>
<p>2023.04.04</p>
<ul>
<li>A function name may not represent a reserved word.</li>
<li>Added Edit -> Unite menu action to separated window.</li>
<li>Added filter utility to Arduino structures widget.</li>
<li>Android and Qt 6.2 LTS.</li>
<li>Application font hinting. Please see Glitch Preferences.</li>
<li>C++17 is the new C++ standard.</li>
<li>Corrected syntax highlighting of #define and #include.</li>
<li>Delete selected items via Backspace and Delete keys if the Alt
  modifier key is also pressed.</li>
<li>Denote the number of objects in Find Objects status bar.</li>
<li>Disable Border Color property for arrow object.</li>
<li>Disable a diagram's tool bar's context menu.</li>
<li>Display categories icons for new diagrams.</li>
<li>Do not display a disconnect symbol over a disconnected port.</li>
<li>Docked widget property editors. Please see Glitch Preferences.</li>
<li>Enabled sorting of Canvas Settings -> Source View -> table.</li>
<li>Highlight an invalid variable name after editing completes.</li>
<li>Native drawing of boolean operators.</li>
<li>New initial color for arrow object.</li>
<li>Removed QPainter::HighQualityAntialiasing render hint.</li>
<li>Replaced Shift modifier with Control modifier: disconnect wires,
  move objects via arrow keys.</li>
<li>SQLite database of new diagram was missing keyword_colors field.</li>
<li>Sensible notification of undefined source file.</li>
<li>Separate pages via Control and drag.</li>
<li>Show tool bars for new diagrams.</li>
<li>Stack widgets. New z-value property.</li>
<li>Super render hints for arrow object.</li>
<li>The main window's title will be incorrect if a non-current diagram
  is saved.</li>
<li>Underline function clone name if parent function has members.</li>
</ul>
<p>2023.02.28</p>
<ul>
<li>Categories icons.</li>
<li>Corrected buddies (mnemonics).</li>
<li>Do not display order indicators for decorative widgets.</li>
<li>Eliminated background color of arrow widget.</li>
<li>Improved adjust-size of constant object.</li>
<li>New colorful icons.</li>
<li>Qt 5.15.8 LTS for MacOS.</li>
<li>Retain scrollbar values in Source View.</li>
<li>Set cursor position to 0 after setText() for textfields.</li>
<li>Source preview highlighting. See also Canvas Settings.</li>
<li>The first Android release was available in 2023.02.10!</li>
</ul>
<p>2023.02.10</p>
<ul>
<li>Arduino Documentation and Release Notes window titles.</li>
<li>Corrected changed states of views. Function object revisions may cause
  multiple signals.</li>
<li>Do not create a source file after a canvas's settings are applied.</li>
<li>Intelligent tree collapse / expand.</li>
<li>Object border color via context menu item.</li>
<li>Set object opacity after a paste event.</li>
<li>Source view. See Project -> Generate Source View.</li>
</ul>
<p>2023.02.05</p>
<ul>
<li>Floating context menu items are now sorted by name.</li>
<li>Horizontal arrow object for decorating diagrams.</li>
<li>Qt 5.5.1 corrections.</li>
</ul>
<p>2023.01.15</p>
<ul>
<li>Copy Object context menu item will not require that the object be
  selected.</li>
<li>Download official version information and display it in About.</li>
<li>Enable Set Port Colors menu item if the parent widget has input or
  output.</li>
<li>Include diagram name in Find Objects.</li>
<li>Save option from object views.</li>
<li>Show object floating menu via context menu action.</li>
</ul>
<p>2023.01.01</p>
<ul>
<li>Added Find action to separated window.</li>
<li>Arduino IDE Linux path was incorrect.</li>
<li>Close button added in Find Objects.</li>
<li>Double-click event in Find Objects.</li>
<li>New widget heights.</li>
<li>Object input and output port colors.</li>
<li>Open the Arduino IDE even if the output file is not defined.</li>
<li>Optionally synchronize Find Objects with parent diagram.</li>
<li>QKeyEvent::modifiers() is not reliable. Replaced with
  QGuiApplication::keyboardModifiers().</li>
<li>Scroll-on-selection in diagrams.</li>
<li>Select a widget if its combination box is clicked on by the left mouse
  button.</li>
<li>Simplified flow-control conditional text.</li>
</ul>
<p>2022.11.30</p>
<ul>
<li>Detect clean states of redo / undo stacks and adjust states of parent
  diagrams.</li>
<li>Project IDE. Please see Project menu.</li>
<li>Renamed Generate menu to Project.</li>
<li>Special copying of loop() and setup() contents.</li>
</ul>
<p>2022.11.25</p>
<ul>
<li>Copy context-menu action.</li>
<li>Detect clean-state of redo / undo stack and adjust diagram state.</li>
<li>Establish wires after undo events.</li>
<li>Fancy collapse widgets for trees.</li>
<li>New Position column in Find Objects. Proper sorting is included.</li>
<li>Paste context-menu actions attached to diagrams.</li>
</ul>
<p>2022.11.20</p>
<ul>
<li>Connect (wire) copied items if necessary.</li>
<li>Copy correct locations of function clones.</li>
<li>New selection color canvas property.</li>
<li>Optimization level three.</li>
<li>Prevent artificial redo / undo stack entries caused by copying and
  pasting of objects.</li>
</ul>
<p>2022.11.15</p>
<ul>
<li>Application fonts.</li>
<li>Glitch Preferences. Please see Windows -> Glitch Preferences.</li>
<li>New Canvas Settings layout.</li>
<li>Prepare for future translations.</li>
<li>Wire color alphas.</li>
<li>Wire widths.</li>
</ul>
<p>2022.11.10</p>
<ul>
<li>Find objects.</li>
<li>Minimum widget heights.</li>
<li>Precise object names (context menus, find).</li>
<li>Prevent pasting of object beyond scene boundary.</li>
<li>Qt 5.15.7 LTS for MacOS.</li>
<li>Removed logical operator as it duplicates boolean operator.</li>
</ul>
<p>2022.11.05</p>
<ul>
<li>Initial release!</li>
</ul>
<p>2004.09.25</p>
<ul>
<li>A splendid idea is born.</li>
</ul>
