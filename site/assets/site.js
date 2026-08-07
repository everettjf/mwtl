(() => {
  const keywords = new Set([
    "alignas", "auto", "bool", "break", "case", "catch", "class", "const",
    "constexpr", "continue", "default", "delete", "do", "else", "explicit",
    "false", "final", "for", "if", "namespace", "new", "noexcept", "nullptr",
    "operator", "override", "private", "protected", "public", "requires", "return",
    "static", "struct", "template", "this", "throw", "true", "try", "typename",
    "using", "virtual", "void", "while"
  ]);
  const types = new Set([
    "DWORD", "HINSTANCE", "HRESULT", "HWND", "LRESULT", "PWSTR", "UINT", "WPARAM",
    "Application", "Button", "CommandEvent", "ControlId", "EventResult", "ImageList",
    "KeyEvent", "Label", "ListView", "MainWindow", "RectDip", "TaskDialogResult",
    "TimerId", "TreeView", "UiTimer", "WindowBase"
  ]);
  const tokenPattern = /\/\/[^\n]*|\/\*[\s\S]*?\*\/|L?"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|^\s*#[^\n]*|\b(?:[A-Za-z_]\w*|\d+(?:\.\d+)?)\b/gm;

  document.querySelectorAll("pre code").forEach((code) => {
    const source = code.textContent;
    const fragment = document.createDocumentFragment();
    let cursor = 0;
    for (const match of source.matchAll(tokenPattern)) {
      fragment.append(document.createTextNode(source.slice(cursor, match.index)));
      const token = match[0];
      let kind = "";
      if (token.startsWith("//") || token.startsWith("/*")) kind = "comment";
      else if (token.trimStart().startsWith("#")) kind = "preprocessor";
      else if (/^L?["']/.test(token)) kind = "string";
      else if (/^\d/.test(token)) kind = "number";
      else if (keywords.has(token)) kind = "keyword";
      else if (types.has(token) || /^[A-Z][A-Za-z0-9_]*$/.test(token)) kind = "type";

      if (kind) {
        const span = document.createElement("span");
        span.className = `syntax-${kind}`;
        span.textContent = token;
        fragment.append(span);
      } else {
        fragment.append(document.createTextNode(token));
      }
      cursor = match.index + token.length;
    }
    fragment.append(document.createTextNode(source.slice(cursor)));
    code.replaceChildren(fragment);
  });
})();
