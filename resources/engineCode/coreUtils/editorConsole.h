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

	// for manipulating engine state
	engine * parent = nullptr;

	// the editor itself
	TextEditor editor;

	// console data
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

	editor.Render( "Editor" );
}

inline void editorConsole::setupConsole () {
	consoleCommands.push_back( "help" ); // dump command list
	consoleCommands.push_back( "compile" ); // compile the contents of the editor
	consoleCommands.push_back( "list" ); // list out the saved scripts
	consoleCommands.push_back( "load" ); // load a script from the saved scripts
	consoleCommands.push_back( "save" ); // save the current script to the saved scripts
	consoleCommands.push_back( "history" ); // list out all the commands that have been entered


}

inline void editorConsole::setupEditor () {
	auto lang = TextEditor::LanguageDefinition::GLSL();
	editor.SetLanguageDefinition( lang );
	editor.SetPalette( TextEditor::GetDarkPalette() );

	// I think this is going to expose the whole shader, for flexibility
	
}

#endif
