// #include "../includes.h"
#include "../engine.h"

#ifndef CONSOLE_H
#define CONSOLE_H


class editorConsole {
public:
	editorConsole() {
		setupConsole();
		setupEditor();
	}

	void setupConsole();
	void setupEditor();
	void draw();
	void drawConfig();

	// config flags ... tbd
		// palette switching
	// using the selected font - provide some monospace options - tbd
	// resize any given font by redeclaring with the new point size ...
	int pickedFont = 0;
	std::vector< ImFont * > fonts;

	// load from file or put it in setupEditor - tbd
	std::string userShaderBase;

	engine * parent = nullptr; // for manipulating engine state
	void setParent( engine * myParent ) { parent = myParent; }

	// the editor itself
	TextEditor editor;

	// console data + manip
	void clearConsoleHistory();
	void clearConsoleLog();
	void addConsoleHistoryItem( string item );
	void executeCommand( string command );
	int textEditCallback( ImGuiInputTextCallbackData *data ); // tbd

	char consoleInputBuffer[ 256 ];
	std::vector< string > consoleCommands;
	std::vector< string > consoleHistory; // list of commands that have been entered ( for completion )
	std::vector< string > consoleItems; // list of display elements
};

inline void editorConsole::draw () {

	auto cpos = editor.GetCursorPosition();
	ImGui::Text( "%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1,
		cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(),
		"User Shader" ); // show User Shader ( Basic ) or User Shader ( Advanced )

	// push selected monospace font
	editor.Render( "Editor", ImVec2( -FLT_MIN, ( 2.0f * ImGui::GetWindowSize().y ) / 3.0f + 18.0f ) );
	// pop the font

	if ( ImGui::SmallButton( " Compile and Run " ) ) {

	}
	ImGui::SameLine();
	if ( ImGui::SmallButton( " Clear Editor " ) ) {
		// reset the contents of the editor to the base shader
	}
	ImGui::SameLine();
	if ( ImGui::SmallButton( " Clear Console " ) ) {
		clearConsoleLog();
	}
	// slider for the sample count? tbd

	ImGui::Separator(); // draw the console contents
	const float footerHeightReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	ImGui::BeginChild( "ScrollingRegion", ImVec2( 0, -footerHeightReserve ), false, ImGuiWindowFlags_HorizontalScrollbar );
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4, 1 ) ); // Tighten spacing
	ImGui::PushTextWrapPos( ImGui::GetWindowSize().x );
	for ( unsigned int i = 0; i < consoleItems.size(); i++ ) {
		ImGui::TextUnformatted( consoleItems[ i ].c_str() );
	}
	ImGui::PopTextWrapPos();
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	bool reclaimFocus = false;
	ImGuiInputTextFlags flags =
		ImGuiInputTextFlags_EnterReturnsTrue |
		ImGuiInputTextFlags_CallbackCompletion |
		ImGuiInputTextFlags_CallbackHistory;

	ImGui::PushItemWidth( ImGui::GetWindowWidth() );
	if ( ImGui::InputText( "##", consoleInputBuffer, IM_ARRAYSIZE( consoleInputBuffer ), flags ) ) {
		executeCommand( string( consoleInputBuffer ) );
		reclaimFocus = true;
	}
	ImGui::PopItemWidth();

	ImGui::SetItemDefaultFocus(); // Auto-focus on window apparition
	if ( reclaimFocus ) {
		ImGui::SetKeyboardFocusHere( -1 ); // Auto focus previous widget
	}
}

inline void editorConsole::drawConfig () {
	// setting the config on the editor
	parent->OrangeText( "Configuration" );



}

inline void editorConsole::setupConsole () {
	consoleCommands.push_back( "help" ); // dump command list
	consoleCommands.push_back( "compile" ); // compile the contents of the editor
	consoleCommands.push_back( "list" ); // list out the saved scripts
	consoleCommands.push_back( "load" ); // load a script from the saved scripts
	consoleCommands.push_back( "save" ); // save the current script to the saved scripts
	consoleCommands.push_back( "history" ); // list out all the commands that have been entered
	consoleCommands.push_back( "clear" ); // clear the history

	consoleItems.push_back( "TEST" );
	consoleItems.push_back( "TEST" );
	consoleItems.push_back( "TEST" );
	consoleItems.push_back( "TEST" );
	consoleItems.push_back( "TEST" );
	consoleItems.push_back( "TEST" );
	consoleItems.push_back( "THE NUT" );
	consoleItems.push_back( "THE NUT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT" );
	consoleItems.push_back( "THE NUT, MY BUTT, I DIE" );
}

inline void editorConsole::setupEditor () {
	auto lang = TextEditor::LanguageDefinition::GLSL();
	editor.SetLanguageDefinition( lang );
	editor.SetPalette( TextEditor::GetDarkPalette() );

	// I think this is going to expose the whole shader, for added flexibility
		// make this configurable? mode select between simple / full? tbd

}

inline void editorConsole::clearConsoleHistory() {
	consoleHistory.clear();
}

inline void editorConsole::clearConsoleLog() {
	consoleItems.clear();
	consoleItems.push_back( currentTimeAndDate() + "Welcome to the Voraldo 13 User Shader Console.\n  'help' for command list. " );
}

inline void editorConsole::addConsoleHistoryItem( string item ) {
	consoleItems.push_back( item );
}

inline void editorConsole::executeCommand( string command ) {
	cout << "the command is: " << command << endl;

	if ( command == "clear" ) {
		clearConsoleHistory();
	} else if ( command == "help" ) {

	} else if ( command == "history" ) {

	} else if ( command == "compile" ) {

	} else if ( command == "save" ) { // need to only look oat the first couple chars, tbd

	} else if ( command == "load" ) { // same

	} else if ( command == "list" ) {

	} else {
		// report command + command not found
	}
}

#endif
