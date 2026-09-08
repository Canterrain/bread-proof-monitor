#include "text_utils.h"

String htmlEscape(String input) {
  input.replace("&", "&amp;");
  input.replace("<", "&lt;");
  input.replace(">", "&gt;");
  input.replace("\"", "&quot;");
  return input;
}

String jsEscape(String input) {
  input.replace("\\", "\\\\");
  input.replace("\"", "\\\"");
  input.replace("\n", "\\n");
  return input;
}

String jsonEscape(String input) {
  input.replace("\\", "\\\\");
  input.replace("\"", "\\\"");
  input.replace("\n", "\\n");
  return input;
}
