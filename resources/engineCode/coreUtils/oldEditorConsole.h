
// this is the old implementation - a lot of room for improvement

// assumes an already open window
// locally declared class, static instance held to keep the info
struct consoleclass {
  char InputBuf[256];
  ImVector<char *> Items;
  ImVector<const char *> Commands;
  ImVector<char *> History;
  int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
  engine *parent = NULL;
  ImGuiTextFilter Filter;
  bool AutoScroll;
  bool ScrollToBottom;
  TextEditor editor;

  char text[1 << 18];
  char origtext[265] =
	  "irec is_inside(){  // check Documentation tab for details \n\n"
	  " irec temp;\n\n"
	  " temp.draw  = false;   // is this voxel's value going to change?\n"
	  " temp.color = vec4(0); // what should its color be?\n"
	  " temp.mask  = 0;       // how much do you wish to mask?\n\n"
	  " return temp;\n\n"
	  "}";

  consoleclass() {
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	strcpy(text, origtext);

	Commands.push_back("help"); // dump command list
	Commands.push_back("man");  // list all function names
	Commands.push_back("compile");
	Commands.push_back("list"); // list of saves in scripts/
	Commands.push_back("load");
	Commands.push_back("save");
	Commands.push_back("history");

	AutoScroll = true;
	ScrollToBottom = true;
  }
  ~consoleclass() {
	ClearLog();
	for (int i = 0; i < History.Size; i++)
	  free(History[i]);
  }

  // Portable helpers
  static int Stricmp(const char *s1, const char *s2) {
	int d;
	while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
	  s1++;
	  s2++;
	}
	return d;
  }
  static int Strnicmp(const char *s1, const char *s2, int n) {
	int d = 0;
	while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
	  s1++;
	  s2++;
	  n--;
	}
	return d;
  }
  static char *Strdup(const char *s) {
	size_t len = strlen(s) + 1;
	void *buf = malloc(len);
	IM_ASSERT(buf);
	return (char *)memcpy(buf, (const void *)s, len);
  }

  static void Strtrim(char *s) {
	char *str_end = s + strlen(s);
	while (str_end > s && str_end[-1] == ' ')
	  str_end--;
	*str_end = 0;
  }

  void ClearLog() {
	for (int i = 0; i < Items.Size; i++)
	  free(Items[i]);
	Items.clear();
	AddLog(std::string(
			   current_time_and_date() +
			   std::string("Welcome to the Voraldo v1.2 User Shader Console. "
						   "\n'help' for command list."))
			   .c_str());
  }

  void AddLog(const char *fmt, ...) IM_FMTARGS(2) {
	// FIXME-OPT
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(Strdup(buf));
  }

  void Draw(const char *title, bool *p_open) {

	ImGui::SameLine();
	if (ImGui::SmallButton(" Clear Console ")) {
	  ClearLog();
	}

	ImGui::SameLine();
	ImGui::SliderInt("Samples", &user_samples, 1, 64);

	ImGui::Separator();

	// Reserve enough left-over height for 1 separator + 1 input text
	ImGui::PushItemWidth(ImGui::GetWindowWidth());

	const float footer_height_to_reserve =
		ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing() +
		8;
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve),
					  false, ImGuiWindowFlags_HorizontalScrollbar);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
						ImVec2(4, 1)); // Tighten spacing

	for (int i = 0; i < Items.Size; i++) {
	  const char *item = Items[i];
	  // if (!Filter.PassFilter(item))
		// continue;

	  // Normally you would store more information in your item than just a
	  // string. (e.g. make Items[] an array of structure, store color/type
	  // etc.)
	  // ImVec4 color;
	  // bool has_color = false;
	  // if (strstr(item, "[error]")) {
		// color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
		// has_color = true;
	  // } else if (strncmp(item, "> ", 2) == 0) {
		// color = ImVec4(1.0f, 0.618f, 0.218f, 1.0f);
		// has_color = true;
	  // }
	  // if (has_color)
		// ImGui::PushStyleColor(ImGuiCol_Text, color);
	  // ImGui::TextUnformatted(item);
	  WrappedText(item);
	  // if (has_color)
		// ImGui::PopStyleColor();
	}

	if (ScrollToBottom || ( AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() ) )
	  ImGui::SetScrollHereY(1.0f);
	ScrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags =
		ImGuiInputTextFlags_EnterReturnsTrue |
		ImGuiInputTextFlags_CallbackCompletion |
		ImGuiInputTextFlags_CallbackHistory;

	// ImGui::PushItemWidth(ImGui::GetWindowWidth());

	if (ImGui::InputText(" ", InputBuf, IM_ARRAYSIZE(InputBuf),
						 input_text_flags, &TextEditCallbackStub,
						 (void *)this)) {
	  char *s = InputBuf;
	  Strtrim(s);
	  if (s[0])
		ExecCommand(s);
	  strcpy(s, "");
	  reclaim_focus = true;
	}

	ImGui::PopItemWidth();

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
	  ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
  }

  void ExecCommand(const char *command_line) {
	AddLog("> %s\n", command_line);

	// Insert into history. First find match and delete it so it can be pushed
	// to the back. This isn't trying to be smart or optimal.
	HistoryPos = -1;
	for (int i = History.Size - 1; i >= 0; i--)
	  if (Stricmp(History[i], command_line) == 0) {
		free(History[i]);
		History.erase(History.begin() + i);
		break;
	  }
	History.push_back(Strdup(command_line));

	// Process command
	if (Stricmp(command_line, "clear") == 0) {
	  ClearLog();
	} else if (Stricmp(command_line, "help") == 0) {
	  AddLog("Commands:");
	  for (int i = 0; i < Commands.Size; i++)
		AddLog("- %s", Commands[i]);
	} else if (Stricmp(command_line, "history") == 0) {
	  int first = History.Size - 10;
	  for (int i = first > 0 ? first : 0; i < History.Size; i++)
		AddLog("%3d: %s\n", i, History[i]);
	} else if (Strnicmp(command_line, "man", 3) == 0) {
	  // AddLog("started with man\n");
	  if (Stricmp(command_line, "man") == 0) { // if it is only 'man'
		AddLog("Use this command to access the manual entry for other "
			   "commands.\n'man list' for the list. \n");
	  } else if (Strnicmp(command_line, "man ", 4) ==
				 0) { // properly formatted command
		// handle individual manual entries
		// probably easiest to use a std::map<std::string, std::string>
		// also have to still decide what goes in this header
	  }
	} else if (Strnicmp(command_line, "load ", 5) == 0) {
	  // try to load the string that follows 'load '
	  std::ifstream file("scripts/" + std::string(command_line + 5));
	  std::string loaded{std::istreambuf_iterator<char>(file),
						 std::istreambuf_iterator<char>()};
	  editor.SetText(loaded);
	} else if (Strnicmp(command_line, "save ", 5) == 0) {
	  // try to save the string to file
	  std::ofstream file("scripts/" + std::string(command_line + 5));
	  std::string savetext(editor.GetText());
	  file << savetext;
	} else if (Stricmp(command_line, "list") == 0) {
	  // list out all the files in scripts/
	  struct path_leaf_string {
		std::string
		operator()(const std::filesystem::directory_entry &entry) const {
		  return entry.path().string();
		}
	  };

	  std::vector<std::string> directory_strings;
	  directory_strings.clear();

	  std::filesystem::path p("scripts/");
	  std::filesystem::directory_iterator start(p);
	  std::filesystem::directory_iterator end;

	  std::transform(start, end, std::back_inserter(directory_strings),
					 path_leaf_string());

	  // sort these alphabetically
	  std::sort(directory_strings.begin(), directory_strings.end());
	  for (auto i : directory_strings) {
		AddLog("  %s\n", i.c_str());
	  }
	} else if (Stricmp(command_line, "compile") == 0) {
	  // compile what's in the box
	  AddLog("%s\n",
			parent->GPU_Data.compile_user_script(std::string(text), user_samples).c_str());
	} else {
	  AddLog("'%s' not found.\n", command_line);
	}

	// On command input, we scroll to bottom even if AutoScroll==false
	ScrollToBottom = true;
  }

  // In C++11 you'd be better off using lambdas for this sort of forwarding
  // callbacks
  static int TextEditCallbackStub(ImGuiInputTextCallbackData *data) {
	consoleclass *console = (consoleclass *)data->UserData;
	return console->TextEditCallback(data);
  }
  int TextEditCallback(ImGuiInputTextCallbackData *data) {
	// AddLog("cursor: %d, selection: %d-%d", data->CursorPos,
	// data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag) {
	case ImGuiInputTextFlags_CallbackCompletion: {
	  // Example of TEXT COMPLETION

	  // Locate beginning of current word
	  const char *word_end = data->Buf + data->CursorPos;
	  const char *word_start = word_end;
	  while (word_start > data->Buf) {
		const char c = word_start[-1];
		if (c == ' ' || c == '\t' || c == ',' || c == ';')
		  break;
		word_start--;
	  }

	  // Build a list of candidates
	  ImVector<const char *> candidates;
	  for (int i = 0; i < Commands.Size; i++)
		if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) ==
			0)
		  candidates.push_back(Commands[i]);

	  if (candidates.Size == 0) {
		// No match
		AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
	  } else if (candidates.Size == 1) {
		// Single match. Delete the beginning of the word and replace it
		// entirely so we've got nice casing.
		data->DeleteChars((int)(word_start - data->Buf),
						  (int)(word_end - word_start));
		data->InsertChars(data->CursorPos, candidates[0]);
		data->InsertChars(data->CursorPos, " ");
	  } else {
		// Multiple matches. Complete as much as we can..
		// So inputing "C"+Tab will complete to "CL" then display "CLEAR" and
		// "CLASSIFY" as matches.
		int match_len = (int)(word_end - word_start);
		for (;;) {
		  int c = 0;
		  bool all_candidates_matches = true;
		  for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
			if (i == 0)
			  c = toupper(candidates[i][match_len]);
			else if (c == 0 || c != toupper(candidates[i][match_len]))
			  all_candidates_matches = false;
		  if (!all_candidates_matches)
			break;
		  match_len++;
		}

		if (match_len > 0) {
		  data->DeleteChars((int)(word_start - data->Buf),
							(int)(word_end - word_start));
		  data->InsertChars(data->CursorPos, candidates[0],
							candidates[0] + match_len);
		}

		// List matches
		AddLog("Possible matches:\n");
		for (int i = 0; i < candidates.Size; i++)
		  AddLog("- %s\n", candidates[i]);
	  }

	  break;
	}
	case ImGuiInputTextFlags_CallbackHistory: {
	  // Example of HISTORY
	  const int prev_history_pos = HistoryPos;
	  if (data->EventKey == ImGuiKey_UpArrow) {
		if (HistoryPos == -1)
		  HistoryPos = History.Size - 1;
		else if (HistoryPos > 0)
		  HistoryPos--;
	  } else if (data->EventKey == ImGuiKey_DownArrow) {
		if (HistoryPos != -1)
		  if (++HistoryPos >= History.Size)
			HistoryPos = -1;
	  }

	  // A better implementation would preserve the data on the current input
	  // line along with cursor position.
	  if (prev_history_pos != HistoryPos) {
		const char *history_str =
			(HistoryPos >= 0) ? History[HistoryPos] : "";
		data->DeleteChars(0, data->BufTextLen);
		data->InsertChars(0, history_str);
	  }
	}
	}
	return 0;
  }
};

static bool draw = true;
static consoleclass console;

// first time init
if (console.parent == NULL) {
  // console init
  console.parent = this;

  // text editor
  console.editor.SetLanguageDefinition(
	  TextEditor::LanguageDefinition::GLSL());

  console.editor.SetPalette(TextEditor::GetDarkPalette());
  // console.editor.SetPalette(TextEditor::GetLightPalette());
  // editor.SetPalette(TextEditor::GetRetroBluePalette());

  console.editor.SetText(std::string(console.origtext));
}

// the first part, the editor -
// this c style string holds the contents of the program -

// ImGui::InputTextMultiline(
//     "source", console.text, IM_ARRAYSIZE(console.text),
//     ImVec2(-FLT_MIN, 2 * total_screen_height / 3), // 2/3 of screen height
//     // ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 36), // 36 lines
//     ImGuiInputTextFlags_AllowTabInput);

auto cpos = console.editor.GetCursorPosition();
ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1,
			cpos.mColumn + 1, console.editor.GetTotalLines(),
			console.editor.IsOverwrite() ? "Ovr" : "Ins",
			console.editor.CanUndo() ? "*" : " ",
			console.editor.GetLanguageDefinition().mName.c_str(),
			"User Script");

console.editor.Render("TextEditor",
					  ImVec2(-FLT_MIN, 2 * total_screen_height / 3));

if (ImGui::SmallButton(" Compile and Run ")) {
  // do some compilation
  // report compilation result / errors / timing
  console.AddLog(
	  "%s\n",
	  GPU_Data.compile_user_script(std::string(console.editor.GetText()), user_samples)
		  .c_str());

  // run the shader for every voxel and report timing
  console.AddLog("%s\n", GPU_Data.run_user_script().c_str());
}

ImGui::SameLine();
if (ImGui::SmallButton(" Clear Editor ")) {
  // strcpy(console.text, console.origtext);
  console.editor.SetText(std::string(console.origtext));
}

console.Draw("ex", &draw);
