<h3><b>Summary of Glitch</b></h3>

<ul>
<li>Android, FreeBSD, Linux, Mac, Windows, etc.</li>
<li>Android-friendly design.</li>
<li>Arduino!</li>
<li>Available almost everywhere!</li>
<li>Copy-and-paste.</li>
<li>Diagrams are recorded in portable SQLite databases.</li>
<li>Drag-and-drop.</li>
<li>Extensible with other frameworks.</li>
<li><b>G</b>raphical <b>l</b>ightweight <b>i</b>nteractive <b>t</b>ool and <b>c</b>ompiler <b>h</b>ybrid.
<li>Infinite diagrams.</li>
<li>Modern C++.</li>
<li>Object alignment tools.</li>
<li>Optionally-transparent widgets if provided by local systems.</li>
<li>PowerPC!</li>
<li>Qt 5.15 LTS.</li>
<li>Qt 5.5.1 (PowerPC)!</li>
<li>Qt 6.2 LTS.</li>
<li>Qt 6.5 LTS.</li>
<li>Qt-only dependency.</li>
<li>Raspberry Pi!</li>
<li>Rich redo / undo stack.</li>
<li>Sparc!</li>
</ul>

<h3><b>Release Notes</b></h3>

<p>2023.04.04</p>
<ul>
<li>A function name may not represent a reserved word.</li>
<li>Added Edit -> Unite menu action to separated window.</li>
<li>Added filter utility to Arduino structures widget.</li>
<li>Android and Qt 6.2 LTS.</li>
<li>Application font hinting. Please see Glitch Preferences.</li>
<li>C++17 is the new C++ standard.</li>
<li>Corrected syntax highlighting of #define and #include.</li>
<li>Delete selected items via Backspace and Delete keys if the Alt modifier key is also pressed.</li>
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
<li>Replaced Shift modifier with Control modifier: disconnect wires, move objects via arrow keys.</li>
<li>SQLite database of new diagram was missing keyword_colors field.</li>
<li>Sensible notification of undefined source file.</li>
<li>Separate pages via Control and drag.</li>
<li>Show tool bars for new diagrams.</li>
<li>Stack widgets. New z-value property.</li>
<li>Super render hints for arrow object.</li>
<li>The main window's title will be incorrect if a non-current diagram is saved.</li>
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
<li>Qt 5.15.8 for MacOS.</li>
<li>Retain scrollbar values in Source View.</li>
<li>Set cursor position to 0 after setText() for textfields.</li>
<li>Source preview highlighting. See also Canvas Settings.</li>
<li>The first Android release was available in 2023.02.10!</li>
</ul>
<p>2023.02.10</p>
<ul>
<li>Arduino Documentation and Release Notes window titles.</li>
<li>Corrected changed states of views. Function object revisions may cause multiple signals.</li>
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
<li>Copy Object context menu item will not require that the object be selected.</li>
<li>Download official version information and display it in About.</li>
<li>Enable Set Port Colors menu item if the parent widget has input or output.</li>
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
<li>QKeyEvent::modifiers() is not reliable. Replaced with QGuiApplication::keyboardModifiers().</li>
<li>Scroll-on-selection in diagrams.</li>
<li>Select a widget if its combination box is clicked on by the left mouse button.</li>
<li>Simplified flow-control conditional text.</li>
</ul>
<p>2022.11.30</p>
<ul>
<li>Detect clean states of redo / undo stacks and adjust states of parent diagrams.</li>
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
<li>Prevent artificial redo / undo stack entries caused by copying and pasting of objects.</li>
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
<li>Qt 5.15.7 for MacOS.</li>
<li>Removed logical operator as it duplicates boolean operator.</li>
</ul>
<p>2022.11.05</p>
<ul>
<li>Initial release!</li>
</ul>
