#include "../includes.h"

#ifndef CONSOLE_H
#define CONSOLE_H

class engine; // forward declare

class editorConsole {
public:
	editorConsole() {
		setupConsole();
		setupEditor();
	}

	void setupConsole();
	void setupEditor();
	void draw();

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
	void addConsoleHistoryItem( string item );
	void executeCommand( string command );
	int textEditCallback( ImGuiInputTextCallbackData *data ); // tbd

	std::vector< string > consoleCommands;
	std::vector< string > consoleHistory;
	std::vector< string > consoleItems;
};

inline void editorConsole::draw () {
	auto cpos = editor.GetCursorPosition();
	ImGui::Text( "%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1,
		cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(),
		"User Shader" );

	// push selected font
	editor.Render( "Editor", ImVec2( -FLT_MIN, 2 * ImGui::GetWindowHeight() / 3 ) );
	// pop the font


	if ( ImGui::SmallButton( " Compile and Run " ) ) {

	}
	ImGui::SameLine();
	if ( ImGui::SmallButton( " Clear Editor " ) ) {

	}
	ImGui::SameLine();
	if ( ImGui::SmallButton( " Clear Console " ) ) {
		clearConsoleLog();
	}

	// draw the console contents
}

inline void editorConsole::setupConsole () {
	consoleCommands.push_back( "help" ); // dump command list
	consoleCommands.push_back( "compile" ); // compile the contents of the editor
	consoleCommands.push_back( "list" ); // list out the saved scripts
	consoleCommands.push_back( "load" ); // load a script from the saved scripts
	consoleCommands.push_back( "save" ); // save the current script to the saved scripts
	consoleCommands.push_back( "history" ); // list out all the commands that have been entered
	consoleCommands.push_back( "clear" ); // clear the history
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
	consoleItems.push_back( currentTimeAndDate + "Welcome to the Voraldo 13 User Shader Console.\n  'help' for command list. " );
}

inline void editorConsole::addConsoleHistoryItem( string item ) {
	consoleItems.push_back( item );
}

inline void editorConsole::executeCommand( string command ) {
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
