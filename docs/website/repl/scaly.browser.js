"use strict";
var Scaly = (() => {
  var __defProp = Object.defineProperty;
  var __getOwnPropDesc = Object.getOwnPropertyDescriptor;
  var __getOwnPropNames = Object.getOwnPropertyNames;
  var __hasOwnProp = Object.prototype.hasOwnProperty;
  var __export = (target, all) => {
    for (var name in all)
      __defProp(target, name, { get: all[name], enumerable: true });
  };
  var __copyProps = (to, from, except, desc) => {
    if (from && typeof from === "object" || typeof from === "function") {
      for (let key of __getOwnPropNames(from))
        if (!__hasOwnProp.call(to, key) && key !== except)
          __defProp(to, key, { get: () => from[key], enumerable: !(desc = __getOwnPropDesc(from, key)) || desc.enumerable });
    }
    return to;
  };
  var __toCommonJS = (mod) => __copyProps(__defProp({}, "__esModule", { value: true }), mod);

  // src/browser.ts
  var browser_exports = {};
  __export(browser_exports, {
    evaluate: () => evaluate,
    resetEvaluator: () => resetEvaluator
  });

  // src/model-error.ts
  function calculatePosition(text, offset) {
    let line = 1;
    let column = 1;
    for (let i = 0; i < offset && i < text.length; i++) {
      if (text[i] === "\n") {
        line++;
        column = 1;
      } else {
        column++;
      }
    }
    return { line, column };
  }
  function buildHintLines(text, startOffset, endOffset, startPosition, endPosition) {
    const lines = [];
    let currentLine = 1;
    let lineStart = 0;
    for (let i = 0; i <= text.length; i++) {
      if (i === text.length || text[i] === "\n") {
        if (currentLine >= startPosition.line && currentLine <= endPosition.line) {
          const lineContent = text.slice(lineStart, i);
          lines.push(lineContent);
          let indicator = "";
          for (let col = 0; col < lineContent.length; col++) {
            const pos = lineStart + col;
            if (pos >= startOffset && pos < endOffset) {
              indicator += "^";
            } else {
              indicator += " ";
            }
          }
          lines.push(indicator);
        }
        currentLine++;
        lineStart = i + 1;
        if (currentLine > endPosition.line) break;
      }
    }
    return lines.join("\n");
  }
  function formatModelError(error, sourceText) {
    switch (error._tag) {
      case "Parser": {
        const { file, error: parserError2 } = error;
        if (parserError2._tag === "DifferentSyntax") {
          return `${file}: A different syntax was expected here.`;
        }
        const pos = sourceText ? calculatePosition(sourceText, parserError2.start) : { line: 0, column: 0 };
        let msg = `${file}:${pos.line}:${pos.column}: error: Expected ${parserError2.message}.`;
        if (sourceText) {
          const endPos = calculatePosition(sourceText, parserError2.end);
          msg += "\n" + buildHintLines(sourceText, parserError2.start, parserError2.end, pos, endPos);
        }
        return msg;
      }
      case "Builder": {
        const builderError = error.error;
        const pos = sourceText ? calculatePosition(sourceText, builderError.span.start) : { line: 0, column: 0 };
        const file = builderError.file;
        let message;
        switch (builderError._tag) {
          case "NotImplemented":
            message = `The ${builderError.name} syntax cannot be processed by the modeler yet.`;
            break;
          case "DuplicateName":
            message = "This declaration already exists.";
            break;
          case "NonFunctionSymbolExists":
            message = "This declaration already exists, but not as a function.";
            break;
          case "FunctionSymbolExists":
            message = "This declaration already exists, but as a function.";
            break;
          case "DeInitializerExists":
            message = "A deinitializer has already been defined.";
            break;
          case "InvalidConstant":
            message = "This is an invalid constant.";
            break;
          case "InvalidComponentName":
            message = "The component does not have an identifier as name.";
            break;
          case "ModuleRootMustBeConcept":
            message = "The root definition of a module must be a concept.";
            break;
        }
        let msg = `${file}:${pos.line}:${pos.column}: error: ${message}`;
        if (sourceText) {
          const endPos = calculatePosition(sourceText, builderError.span.end);
          msg += "\n" + buildHintLines(sourceText, builderError.span.start, builderError.span.end, pos, endPos);
        }
        return msg;
      }
    }
  }
  function notImplemented(file, name, span) {
    return { _tag: "Builder", error: { _tag: "NotImplemented", file, name, span } };
  }
  function invalidConstant(file, span) {
    return { _tag: "Builder", error: { _tag: "InvalidConstant", file, span } };
  }
  function parserError(file, error) {
    return { _tag: "Parser", file, error };
  }

  // src/lexer.ts
  var Lexer = class {
    token = { _tag: "Empty" };
    character = null;
    chars;
    index = 0;
    previousPosition = 0;
    position = 0;
    constructor(input) {
      this.chars = input;
      this.readCharacter();
      this.skipWhitespace(true);
    }
    readCharacter() {
      if (this.index < this.chars.length) {
        this.character = this.chars[this.index++];
        this.position++;
      } else {
        this.character = null;
      }
    }
    advance() {
      this.previousPosition = this.position;
      if (this.character === null) return;
      const c = this.character;
      if (c >= "a" && c <= "z" || c >= "A" && c <= "Z") {
        this.token = this.scanIdentifier();
        this.skipWhitespace(false);
        return;
      }
      if (c >= "1" && c <= "9") {
        this.token = this.scanIntegerLiteral(this.index - 1, 0);
        this.skipWhitespace(false);
        return;
      }
      if ("+-*/=%&|~<>".includes(c)) {
        this.token = this.scanOperator();
        this.skipWhitespace(false);
        return;
      }
      if ("})]?.!$#^".includes(c)) {
        this.token = { _tag: "Punctuation", sign: c };
        this.readCharacter();
        this.skipWhitespace(false);
        return;
      }
      if ("{([,".includes(c)) {
        this.token = { _tag: "Punctuation", sign: c };
        this.readCharacter();
        this.skipWhitespace(true);
        return;
      }
      switch (c) {
        case "\n":
          this.token = this.scanLineFeed();
          break;
        case ":":
          this.readCharacter();
          this.token = { _tag: "Colon" };
          break;
        case "0":
          this.token = this.scanNumericLiteral();
          break;
        case "@":
          this.readCharacter();
          this.token = this.scanAttribute();
          break;
        case '"':
          this.token = this.scanStringLiteral();
          break;
        case "'":
          this.token = this.scanCharacterLiteral();
          break;
        case "`":
          this.token = this.scanFragmentLiteral();
          break;
        default:
          this.token = { _tag: "Invalid" };
      }
      this.skipWhitespace(false);
    }
    empty() {
      this.token = { _tag: "Empty" };
    }
    scanLineFeed() {
      while (true) {
        this.readCharacter();
        this.skipWhitespace(false);
        if (this.character === null) return { _tag: "Colon" };
        if (this.character === "\n") continue;
        return { _tag: "Colon" };
      }
    }
    scanIdentifier() {
      const start = this.index - 1;
      let length = 0;
      while (true) {
        if (this.character === null) {
          if (length === 0) return { _tag: "Invalid" };
          return { _tag: "Identifier", name: this.chars.slice(start, start + length) };
        }
        const c = this.character;
        if (c >= "a" && c <= "z" || c >= "A" && c <= "Z" || c >= "0" && c <= "9" || c === "_") {
          this.readCharacter();
          length++;
        } else {
          return { _tag: "Identifier", name: this.chars.slice(start, start + length) };
        }
      }
    }
    scanAttribute() {
      const start = this.index - 1;
      let length = 0;
      while (true) {
        if (this.character === null) {
          if (length === 0) return { _tag: "Invalid" };
          return { _tag: "Attribute", name: this.chars.slice(start, start + length) };
        }
        const c = this.character;
        if (c >= "a" && c <= "z" || c >= "A" && c <= "Z" || c >= "0" && c <= "9" || c === "_") {
          this.readCharacter();
          length++;
        } else {
          return { _tag: "Attribute", name: this.chars.slice(start, start + length) };
        }
      }
    }
    scanOperator() {
      const start = this.index - 1;
      let length = 0;
      while (true) {
        this.readCharacter();
        length++;
        if (this.character === null) {
          return { _tag: "Identifier", name: this.chars.slice(start, start + length) };
        }
        if ("+-*/=%&|^~<>".includes(this.character)) {
          continue;
        }
        return { _tag: "Identifier", name: this.chars.slice(start, start + length) };
      }
    }
    scanStringLiteral() {
      const start = this.index;
      let content = "";
      while (true) {
        this.readCharacter();
        if (this.character === null) return { _tag: "Invalid" };
        if (this.character === '"') {
          this.readCharacter();
          return { _tag: "String", value: content };
        }
        if (this.character === "\\") {
          this.readCharacter();
          if (this.character === null) return { _tag: "Invalid" };
          if (!`"\\'nrt0`.includes(this.character)) return { _tag: "Invalid" };
          content += this.escapeChar(this.character);
        } else {
          content += this.character;
        }
      }
    }
    scanCharacterLiteral() {
      const start = this.index;
      let content = "";
      while (true) {
        this.readCharacter();
        if (this.character === null) return { _tag: "Invalid" };
        if (this.character === "'") {
          this.readCharacter();
          return { _tag: "Character", value: content };
        }
        if (this.character === "\\") {
          this.readCharacter();
          if (this.character === null) return { _tag: "Invalid" };
          if (!`"\\'nrt0`.includes(this.character)) return { _tag: "Invalid" };
          content += this.escapeChar(this.character);
        } else {
          content += this.character;
        }
      }
    }
    scanFragmentLiteral() {
      let content = "";
      while (true) {
        this.readCharacter();
        if (this.character === null) return { _tag: "Invalid" };
        if (this.character === "`") {
          this.readCharacter();
          return { _tag: "Fragment", value: content };
        }
        if (this.character === "\\") {
          this.readCharacter();
          if (this.character === null) return { _tag: "Invalid" };
          if (!"`\\'nrt0".includes(this.character)) return { _tag: "Invalid" };
          content += this.escapeChar(this.character);
        } else {
          content += this.character;
        }
      }
    }
    escapeChar(c) {
      switch (c) {
        case "n":
          return "\n";
        case "r":
          return "\r";
        case "t":
          return "	";
        case "0":
          return "\0";
        default:
          return c;
      }
    }
    scanNumericLiteral() {
      const start = this.index - 1;
      let length = 1;
      this.readCharacter();
      if (this.character === null) {
        return { _tag: "Integer", value: this.chars.slice(start, start + length) };
      }
      const c = this.character;
      if (c >= "0" && c <= "9") {
        return this.scanIntegerLiteral(start, length);
      }
      switch (c) {
        case ".":
          return this.scanFraction(start, length);
        case "E":
        case "e":
          return this.scanExponent(start, length);
        case "x":
          return this.scanHexLiteral(start, length);
        case "B":
          return this.scanBooleanLiteral();
        default:
          return { _tag: "Integer", value: this.chars.slice(start, start + length) };
      }
    }
    scanIntegerLiteral(start, length) {
      while (true) {
        this.readCharacter();
        length++;
        if (this.character === null) {
          return { _tag: "Integer", value: this.chars.slice(start, start + length) };
        }
        const c = this.character;
        if (c >= "0" && c <= "9") continue;
        switch (c) {
          case ".":
            return this.scanFraction(start, length);
          case "E":
          case "e":
            return this.scanExponent(start, length);
          default:
            return { _tag: "Integer", value: this.chars.slice(start, start + length) };
        }
      }
    }
    scanFraction(start, length) {
      while (true) {
        this.readCharacter();
        length++;
        if (this.character === null) {
          return { _tag: "FloatingPoint", value: this.chars.slice(start, start + length) };
        }
        const c = this.character;
        if (c >= "0" && c <= "9") continue;
        if (c === "E" || c === "e") {
          return this.scanExponent(start, length);
        }
        return { _tag: "FloatingPoint", value: this.chars.slice(start, start + length) };
      }
    }
    scanExponent(start, length) {
      while (true) {
        this.readCharacter();
        length++;
        if (this.character === null) {
          return { _tag: "FloatingPoint", value: this.chars.slice(start, start + length) };
        }
        const c = this.character;
        if (c >= "0" && c <= "9") continue;
        return { _tag: "FloatingPoint", value: this.chars.slice(start, start + length) };
      }
    }
    scanHexLiteral(start, length) {
      while (true) {
        this.readCharacter();
        length++;
        if (this.character === null) {
          return { _tag: "Hex", value: this.chars.slice(start, start + length) };
        }
        const c = this.character;
        if (c >= "0" && c <= "9" || c >= "A" && c <= "F" || c >= "a" && c <= "f") {
          continue;
        }
        return { _tag: "Hex", value: this.chars.slice(start, start + length) };
      }
    }
    scanBooleanLiteral() {
      this.readCharacter();
      if (this.character === null) return { _tag: "Invalid" };
      const c = this.character;
      if (c !== "0" && c !== "1") return { _tag: "Invalid" };
      this.readCharacter();
      return { _tag: "Boolean", value: c === "1" };
    }
    skipWhitespace(skipLineFeed) {
      while (true) {
        const c = this.character;
        if (c === null) return;
        if (c === " " || c === "	" || c === "\r") {
          this.readCharacter();
          continue;
        }
        if (c === "\n") {
          if (skipLineFeed) {
            this.readCharacter();
            continue;
          }
          return;
        }
        if (c === "\\") {
          this.readCharacter();
          const c1 = this.character;
          if (c1 === null) return;
          if (c1 === "\r") {
            this.readCharacter();
            this.readCharacter();
            continue;
          }
          if (c1 === "\n") {
            this.readCharacter();
            continue;
          }
          return;
        }
        if (c === ";") {
          this.readCharacter();
          const c2 = this.character;
          if (c2 === null) return;
          if (c2 !== "*") {
            this.readCharacter();
            this.handleSingleLineComment();
          } else {
            this.readCharacter();
            this.handleMultiLineComment();
          }
          continue;
        }
        return;
      }
    }
    handleSingleLineComment() {
      while (true) {
        if (this.character === null) return;
        if (this.character === "\n") {
          this.readCharacter();
          return;
        }
        this.readCharacter();
      }
    }
    handleMultiLineComment() {
      while (true) {
        const c = this.character;
        if (c === null) return;
        if (c === ";") {
          this.readCharacter();
          const c2 = this.character;
          if (c2 === null) return;
          if (c2 === "*") {
            this.readCharacter();
            this.handleMultiLineComment();
            continue;
          }
          continue;
        }
        if (c === "*") {
          this.readCharacter();
          const c3 = this.character;
          if (c3 === null) return;
          if (c3 === ";") {
            this.readCharacter();
            return;
          }
          this.readCharacter();
          continue;
        }
        this.readCharacter();
      }
    }
    // Parser helper methods
    parseKeyword(keyword) {
      if (this.token._tag === "Empty") this.advance();
      if (this.token._tag === "Identifier" && this.token.name === keyword) {
        this.empty();
        return true;
      }
      return false;
    }
    parseIdentifier(keywords) {
      if (this.token._tag === "Empty") this.advance();
      if (this.token._tag === "Identifier") {
        if (keywords.has(this.token.name)) return null;
        const name = this.token.name;
        this.empty();
        return name;
      }
      return null;
    }
    parseAttribute() {
      if (this.token._tag === "Empty") this.advance();
      if (this.token._tag === "Attribute") {
        const name = this.token.name;
        this.empty();
        return name;
      }
      return null;
    }
    parsePunctuation(sign) {
      if (this.token._tag === "Empty") this.advance();
      if (this.token._tag === "Punctuation" && this.token.sign === sign) {
        this.empty();
        return true;
      }
      return false;
    }
    parseColon() {
      if (this.token._tag === "Empty") this.advance();
      if (this.token._tag === "Colon") {
        this.empty();
        return true;
      }
      if (this.token._tag === "Empty") {
        return true;
      }
      return false;
    }
    isAtEnd() {
      return this.character === null;
    }
  };

  // src/parser.ts
  function different() {
    return { ok: false, error: { _tag: "DifferentSyntax" } };
  }
  function invalid(start, end, message) {
    return { ok: false, error: { _tag: "InvalidSyntax", start, end, message } };
  }
  function ok(value) {
    return { ok: true, value };
  }
  var KEYWORDS = /* @__PURE__ */ new Set([
    "break",
    "case",
    "choose",
    "continue",
    "define",
    "deinit",
    "delegate",
    "else",
    "extends",
    "extern",
    "for",
    "function",
    "if",
    "is",
    "init",
    "implement",
    "in",
    "instruction",
    "intrinsic",
    "label",
    "lambda",
    "let",
    "loop",
    "macro",
    "match",
    "module",
    "mutable",
    "operator",
    "procedure",
    "private",
    "return",
    "returns",
    "repeat",
    "set",
    "sizeof",
    "throw",
    "throws",
    "trait",
    "try",
    "union",
    "use",
    "var",
    "when",
    "while",
    "package"
  ]);
  var Parser = class {
    lexer;
    constructor(text) {
      this.lexer = new Lexer(text);
    }
    parseLiteralToken() {
      if (this.lexer.token._tag === "Empty") this.lexer.advance();
      const token = this.lexer.token;
      switch (token._tag) {
        case "String": {
          const ret = {
            _syntax: "StringLiteral",
            start: this.lexer.previousPosition,
            end: this.lexer.position,
            value: token.value
          };
          this.lexer.empty();
          return ok(ret);
        }
        case "Character": {
          const ret = {
            _syntax: "CharacterLiteral",
            start: this.lexer.previousPosition,
            end: this.lexer.position,
            value: token.value
          };
          this.lexer.empty();
          return ok(ret);
        }
        case "Integer": {
          const ret = {
            _syntax: "IntegerLiteral",
            start: this.lexer.previousPosition,
            end: this.lexer.position,
            value: token.value
          };
          this.lexer.empty();
          return ok(ret);
        }
        case "FloatingPoint": {
          const ret = {
            _syntax: "FloatingPointLiteral",
            start: this.lexer.previousPosition,
            end: this.lexer.position,
            value: token.value
          };
          this.lexer.empty();
          return ok(ret);
        }
        case "Hex": {
          const ret = {
            _syntax: "HexLiteral",
            start: this.lexer.previousPosition,
            end: this.lexer.position,
            value: token.value
          };
          this.lexer.empty();
          return ok(ret);
        }
        case "Boolean": {
          const ret = {
            _syntax: "BooleanLiteral",
            start: this.lexer.previousPosition,
            end: this.lexer.position,
            value: token.value
          };
          this.lexer.empty();
          return ok(ret);
        }
        case "Fragment": {
          const ret = {
            _syntax: "FragmentLiteral",
            start: this.lexer.previousPosition,
            end: this.lexer.position,
            value: token.value
          };
          this.lexer.empty();
          return ok(ret);
        }
        default:
          return different();
      }
    }
    parseProgram() {
      const start = this.lexer.previousPosition;
      const fileStart = this.lexer.position;
      const fileResult = this.parseFile();
      if (!fileResult.ok) {
        return fileResult;
      }
      const file = fileResult.value;
      const statementsStart = this.lexer.position;
      let statements = null;
      {
        const statementsResult = this.parseStatementList();
        if (statementsResult.ok) statements = statementsResult.value;
        else if (statementsResult.error._tag === "InvalidSyntax") return statementsResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ProgramSyntax",
        start,
        end,
        file,
        statements
      });
    }
    parseFile() {
      const start = this.lexer.previousPosition;
      const packagesStart = this.lexer.position;
      let packages = null;
      {
        const packagesResult = this.parsePackageList();
        if (packagesResult.ok) packages = packagesResult.value;
        else if (packagesResult.error._tag === "InvalidSyntax") return packagesResult;
      }
      const usesStart = this.lexer.position;
      let uses = null;
      {
        const usesResult = this.parseUseList();
        if (usesResult.ok) uses = usesResult.value;
        else if (usesResult.error._tag === "InvalidSyntax") return usesResult;
      }
      const declarationsStart = this.lexer.position;
      let declarations = null;
      {
        const declarationsResult = this.parseDeclarationList();
        if (declarationsResult.ok) declarations = declarationsResult.value;
        else if (declarationsResult.error._tag === "InvalidSyntax") return declarationsResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "FileSyntax",
        start,
        end,
        packages,
        uses,
        declarations
      });
    }
    parseDeclarationList() {
      const list = [];
      while (true) {
        const result = this.parseDeclaration();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseDeclaration() {
      const start = this.lexer.previousPosition;
      const symbolStart = this.lexer.position;
      const symbolResult = this.parseSymbol();
      if (!symbolResult.ok) {
        return symbolResult;
      }
      const symbol = symbolResult.value;
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "DeclarationSyntax",
        start,
        end,
        symbol
      });
    }
    parseSymbol() {
      {
        const result = this.parsePrivate();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseDefinition();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseFunction();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseOperator();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseTrait();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseMacro();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseModule();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parsePrivate() {
      const start = this.lexer.previousPosition;
      const start_private_1 = this.lexer.previousPosition;
      const success_private_1 = this.lexer.parseKeyword("private");
      if (!success_private_1) return different();
      const export_Start = this.lexer.position;
      const export_Result = this.parseExport();
      if (!export_Result.ok) {
        if (export_Result.error._tag === "InvalidSyntax") return export_Result;
        return invalid(export_Start, this.lexer.position, "a valid Export syntax");
      }
      const export_ = export_Result.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "PrivateSyntax",
        start,
        end,
        export_
      });
    }
    parseExport() {
      {
        const result = this.parseDefinition();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseFunction();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseOperator();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseTrait();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseModule();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseMemberList() {
      const list = [];
      while (true) {
        const result = this.parseMember();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseMember() {
      const start = this.lexer.previousPosition;
      const constituentStart = this.lexer.position;
      const constituentResult = this.parseConstituent();
      if (!constituentResult.ok) {
        return constituentResult;
      }
      const constituent = constituentResult.value;
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "MemberSyntax",
        start,
        end,
        constituent
      });
    }
    parseConstituent() {
      {
        const result = this.parseDefinition();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseFunction();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseProcedure();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseOperator();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseImplement();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseTrait();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseMacro();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseModule();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseDefinition() {
      const start = this.lexer.previousPosition;
      const start_define_1 = this.lexer.previousPosition;
      const success_define_1 = this.lexer.parseKeyword("define");
      if (!success_define_1) return different();
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) {
        return invalid(start_name, this.lexer.position, "an identifier");
      }
      const parametersStart = this.lexer.position;
      let parameters = null;
      {
        const parametersResult = this.parseGenericParameters();
        if (parametersResult.ok) parameters = parametersResult.value;
        else if (parametersResult.error._tag === "InvalidSyntax") return parametersResult;
      }
      const start_colon_4 = this.lexer.previousPosition;
      const success_colon_4 = this.lexer.parseColon();
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const concept_Start = this.lexer.position;
      const concept_Result = this.parseConcept();
      if (!concept_Result.ok) {
        if (concept_Result.error._tag === "InvalidSyntax") return concept_Result;
        return invalid(concept_Start, this.lexer.position, "a valid Concept syntax");
      }
      const concept_ = concept_Result.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "DefinitionSyntax",
        start,
        end,
        name,
        parameters,
        attributes,
        concept_
      });
    }
    parseGenericParameters() {
      const start = this.lexer.previousPosition;
      const start_left_bracket_1 = this.lexer.previousPosition;
      const success_left_bracket_1 = this.lexer.parsePunctuation("[");
      if (!success_left_bracket_1) return different();
      const parametersStart = this.lexer.position;
      const parametersResult = this.parseGenericParameterList();
      if (!parametersResult.ok) {
        if (parametersResult.error._tag === "InvalidSyntax") return parametersResult;
        return invalid(parametersStart, this.lexer.position, "a valid GenericParameter syntax");
      }
      const parameters = parametersResult.value;
      const start_right_bracket_3 = this.lexer.previousPosition;
      const success_right_bracket_3 = this.lexer.parsePunctuation("]");
      if (!success_right_bracket_3) {
        return invalid(start_right_bracket_3, this.lexer.position, "]");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "GenericParametersSyntax",
        start,
        end,
        parameters
      });
    }
    parseGenericParameterList() {
      const list = [];
      while (true) {
        const result = this.parseGenericParameter();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseGenericParameter() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) return different();
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const start_comma_3 = this.lexer.previousPosition;
      const success_comma_3 = this.lexer.parsePunctuation(",");
      const end = this.lexer.position;
      return ok({
        _syntax: "GenericParameterSyntax",
        start,
        end,
        name,
        attributes
      });
    }
    parseConcept() {
      {
        const result = this.parseClass();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseNamespace();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseUnion();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseConstant();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseDelegate();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseIntrinsic();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseClass() {
      const start = this.lexer.previousPosition;
      const structureStart = this.lexer.position;
      const structureResult = this.parseStructure();
      if (!structureResult.ok) {
        return structureResult;
      }
      const structure = structureResult.value;
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const bodyStart = this.lexer.position;
      let body = null;
      {
        const bodyResult = this.parseBody();
        if (bodyResult.ok) body = bodyResult.value;
        else if (bodyResult.error._tag === "InvalidSyntax") return bodyResult;
      }
      const start_colon_4 = this.lexer.previousPosition;
      const success_colon_4 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "ClassSyntax",
        start,
        end,
        structure,
        body
      });
    }
    parseBody() {
      const start = this.lexer.previousPosition;
      const start_left_curly_1 = this.lexer.previousPosition;
      const success_left_curly_1 = this.lexer.parsePunctuation("{");
      if (!success_left_curly_1) return different();
      const usesStart = this.lexer.position;
      let uses = null;
      {
        const usesResult = this.parseUseList();
        if (usesResult.ok) uses = usesResult.value;
        else if (usesResult.error._tag === "InvalidSyntax") return usesResult;
      }
      const initsStart = this.lexer.position;
      let inits = null;
      {
        const initsResult = this.parseInitList();
        if (initsResult.ok) inits = initsResult.value;
        else if (initsResult.error._tag === "InvalidSyntax") return initsResult;
      }
      const deInitStart = this.lexer.position;
      let deInit = null;
      {
        const deInitResult = this.parseDeInit();
        if (deInitResult.ok) deInit = deInitResult.value;
        else if (deInitResult.error._tag === "InvalidSyntax") return deInitResult;
      }
      const membersStart = this.lexer.position;
      let members = null;
      {
        const membersResult = this.parseMemberList();
        if (membersResult.ok) members = membersResult.value;
        else if (membersResult.error._tag === "InvalidSyntax") return membersResult;
      }
      const start_right_curly_6 = this.lexer.previousPosition;
      const success_right_curly_6 = this.lexer.parsePunctuation("}");
      if (!success_right_curly_6) {
        return invalid(start_right_curly_6, this.lexer.position, "}");
      }
      const start_colon_7 = this.lexer.previousPosition;
      const success_colon_7 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "BodySyntax",
        start,
        end,
        uses,
        inits,
        deInit,
        members
      });
    }
    parseNamespace() {
      const start = this.lexer.previousPosition;
      const start_left_curly_1 = this.lexer.previousPosition;
      const success_left_curly_1 = this.lexer.parsePunctuation("{");
      if (!success_left_curly_1) return different();
      const usesStart = this.lexer.position;
      let uses = null;
      {
        const usesResult = this.parseUseList();
        if (usesResult.ok) uses = usesResult.value;
        else if (usesResult.error._tag === "InvalidSyntax") return usesResult;
      }
      const declarationsStart = this.lexer.position;
      let declarations = null;
      {
        const declarationsResult = this.parseDeclarationList();
        if (declarationsResult.ok) declarations = declarationsResult.value;
        else if (declarationsResult.error._tag === "InvalidSyntax") return declarationsResult;
      }
      const start_right_curly_4 = this.lexer.previousPosition;
      const success_right_curly_4 = this.lexer.parsePunctuation("}");
      if (!success_right_curly_4) {
        return invalid(start_right_curly_4, this.lexer.position, "}");
      }
      const start_colon_5 = this.lexer.previousPosition;
      const success_colon_5 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "NamespaceSyntax",
        start,
        end,
        uses,
        declarations
      });
    }
    parseUnion() {
      const start = this.lexer.previousPosition;
      const start_union_1 = this.lexer.previousPosition;
      const success_union_1 = this.lexer.parseKeyword("union");
      if (!success_union_1) return different();
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const start_left_paren_3 = this.lexer.previousPosition;
      const success_left_paren_3 = this.lexer.parsePunctuation("(");
      if (!success_left_paren_3) {
        return invalid(start_left_paren_3, this.lexer.position, "(");
      }
      const variantsStart = this.lexer.position;
      const variantsResult = this.parseVariantList();
      if (!variantsResult.ok) {
        if (variantsResult.error._tag === "InvalidSyntax") return variantsResult;
        return invalid(variantsStart, this.lexer.position, "a valid Variant syntax");
      }
      const variants = variantsResult.value;
      const start_right_paren_5 = this.lexer.previousPosition;
      const success_right_paren_5 = this.lexer.parsePunctuation(")");
      if (!success_right_paren_5) {
        return invalid(start_right_paren_5, this.lexer.position, ")");
      }
      const start_colon_6 = this.lexer.previousPosition;
      const success_colon_6 = this.lexer.parseColon();
      const bodyStart = this.lexer.position;
      let body = null;
      {
        const bodyResult = this.parseBody();
        if (bodyResult.ok) body = bodyResult.value;
        else if (bodyResult.error._tag === "InvalidSyntax") return bodyResult;
      }
      const start_colon_8 = this.lexer.previousPosition;
      const success_colon_8 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "UnionSyntax",
        start,
        end,
        variants,
        body
      });
    }
    parseVariantList() {
      const list = [];
      while (true) {
        const result = this.parseVariant();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseVariant() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) return different();
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const annotationStart = this.lexer.position;
      let annotation = null;
      {
        const annotationResult = this.parseTypeAnnotation();
        if (annotationResult.ok) annotation = annotationResult.value;
        else if (annotationResult.error._tag === "InvalidSyntax") return annotationResult;
      }
      const start_comma_4 = this.lexer.previousPosition;
      const success_comma_4 = this.lexer.parsePunctuation(",");
      const start_colon_5 = this.lexer.previousPosition;
      const success_colon_5 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "VariantSyntax",
        start,
        end,
        name,
        attributes,
        annotation
      });
    }
    parseConstant() {
      const start = this.lexer.previousPosition;
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        return typeResult;
      }
      const type = typeResult.value;
      const operationStart = this.lexer.position;
      let operation = null;
      {
        const operationResult = this.parseOperandList();
        if (operationResult.ok) operation = operationResult.value;
        else if (operationResult.error._tag === "InvalidSyntax") return operationResult;
      }
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "ConstantSyntax",
        start,
        end,
        type,
        operation
      });
    }
    parseDelegate() {
      const start = this.lexer.previousPosition;
      const start_delegate_1 = this.lexer.previousPosition;
      const success_delegate_1 = this.lexer.parseKeyword("delegate");
      if (!success_delegate_1) return different();
      const parametersStart = this.lexer.position;
      let parameters = null;
      {
        const parametersResult = this.parseParameterSet();
        if (parametersResult.ok) parameters = parametersResult.value;
        else if (parametersResult.error._tag === "InvalidSyntax") return parametersResult;
      }
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const resultStart = this.lexer.position;
      let result = null;
      {
        const resultResult = this.parseReturns();
        if (resultResult.ok) result = resultResult.value;
        else if (resultResult.error._tag === "InvalidSyntax") return resultResult;
      }
      const errorStart = this.lexer.position;
      let error = null;
      {
        const errorResult = this.parseThrows();
        if (errorResult.ok) error = errorResult.value;
        else if (errorResult.error._tag === "InvalidSyntax") return errorResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "DelegateSyntax",
        start,
        end,
        parameters,
        attributes,
        result,
        error
      });
    }
    parseGenericArguments() {
      const start = this.lexer.previousPosition;
      const start_left_bracket_1 = this.lexer.previousPosition;
      const success_left_bracket_1 = this.lexer.parsePunctuation("[");
      if (!success_left_bracket_1) return different();
      const genericsStart = this.lexer.position;
      let generics = null;
      {
        const genericsResult = this.parseGenericArgumentList();
        if (genericsResult.ok) generics = genericsResult.value;
        else if (genericsResult.error._tag === "InvalidSyntax") return genericsResult;
      }
      const start_right_bracket_3 = this.lexer.previousPosition;
      const success_right_bracket_3 = this.lexer.parsePunctuation("]");
      if (!success_right_bracket_3) {
        return invalid(start_right_bracket_3, this.lexer.position, "]");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "GenericArgumentsSyntax",
        start,
        end,
        generics
      });
    }
    parseGenericArgumentList() {
      const list = [];
      while (true) {
        const result = this.parseGenericArgument();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseGenericArgument() {
      const start = this.lexer.previousPosition;
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        return typeResult;
      }
      const type = typeResult.value;
      const start_comma_2 = this.lexer.previousPosition;
      const success_comma_2 = this.lexer.parsePunctuation(",");
      const end = this.lexer.position;
      return ok({
        _syntax: "GenericArgumentSyntax",
        start,
        end,
        type
      });
    }
    parseOptional() {
      const start = this.lexer.previousPosition;
      const start_question_1 = this.lexer.previousPosition;
      const success_question_1 = this.lexer.parsePunctuation("?");
      if (!success_question_1) return different();
      const end = this.lexer.position;
      return ok({
        _syntax: "OptionalSyntax",
        start,
        end
      });
    }
    parseParameterSet() {
      {
        const result = this.parseParameters();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseType();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseParameters() {
      const start = this.lexer.previousPosition;
      const start_left_paren_1 = this.lexer.previousPosition;
      const success_left_paren_1 = this.lexer.parsePunctuation("(");
      if (!success_left_paren_1) return different();
      const itemsStart = this.lexer.position;
      let items = null;
      {
        const itemsResult = this.parseItemList();
        if (itemsResult.ok) items = itemsResult.value;
        else if (itemsResult.error._tag === "InvalidSyntax") return itemsResult;
      }
      const start_right_paren_3 = this.lexer.previousPosition;
      const success_right_paren_3 = this.lexer.parsePunctuation(")");
      if (!success_right_paren_3) {
        return invalid(start_right_paren_3, this.lexer.position, ")");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ParametersSyntax",
        start,
        end,
        items
      });
    }
    parseItemList() {
      const list = [];
      while (true) {
        const result = this.parseItem();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseItem() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) return different();
      const annotationStart = this.lexer.position;
      let annotation = null;
      {
        const annotationResult = this.parseTypeAnnotation();
        if (annotationResult.ok) annotation = annotationResult.value;
        else if (annotationResult.error._tag === "InvalidSyntax") return annotationResult;
      }
      const start_comma_3 = this.lexer.previousPosition;
      const success_comma_3 = this.lexer.parsePunctuation(",");
      const start_colon_4 = this.lexer.previousPosition;
      const success_colon_4 = this.lexer.parseColon();
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const start_colon_6 = this.lexer.previousPosition;
      const success_colon_6 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "ItemSyntax",
        start,
        end,
        name,
        annotation,
        attributes
      });
    }
    parseReturns() {
      const start = this.lexer.previousPosition;
      const start_returns_1 = this.lexer.previousPosition;
      const success_returns_1 = this.lexer.parseKeyword("returns");
      if (!success_returns_1) return different();
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        if (typeResult.error._tag === "InvalidSyntax") return typeResult;
        return invalid(typeStart, this.lexer.position, "a valid Type syntax");
      }
      const type = typeResult.value;
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ReturnsSyntax",
        start,
        end,
        type,
        attributes
      });
    }
    parseThrows() {
      const start = this.lexer.previousPosition;
      const start_throws_1 = this.lexer.previousPosition;
      const success_throws_1 = this.lexer.parseKeyword("throws");
      if (!success_throws_1) return different();
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        if (typeResult.error._tag === "InvalidSyntax") return typeResult;
        return invalid(typeStart, this.lexer.position, "a valid Type syntax");
      }
      const type = typeResult.value;
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ThrowsSyntax",
        start,
        end,
        type,
        attributes
      });
    }
    parseUseList() {
      const list = [];
      while (true) {
        const result = this.parseUse();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseUse() {
      const start = this.lexer.previousPosition;
      const start_use_1 = this.lexer.previousPosition;
      const success_use_1 = this.lexer.parseKeyword("use");
      if (!success_use_1) return different();
      const nameStart = this.lexer.position;
      const nameResult = this.parseName();
      if (!nameResult.ok) {
        if (nameResult.error._tag === "InvalidSyntax") return nameResult;
        return invalid(nameStart, this.lexer.position, "a valid Name syntax");
      }
      const name = nameResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "UseSyntax",
        start,
        end,
        name
      });
    }
    parseImplement() {
      const start = this.lexer.previousPosition;
      const start_implement_1 = this.lexer.previousPosition;
      const success_implement_1 = this.lexer.parseKeyword("implement");
      if (!success_implement_1) return different();
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        if (typeResult.error._tag === "InvalidSyntax") return typeResult;
        return invalid(typeStart, this.lexer.position, "a valid Type syntax");
      }
      const type = typeResult.value;
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const start_colon_4 = this.lexer.previousPosition;
      const success_colon_4 = this.lexer.parseColon();
      const start_left_curly_5 = this.lexer.previousPosition;
      const success_left_curly_5 = this.lexer.parsePunctuation("{");
      if (!success_left_curly_5) {
        return invalid(start_left_curly_5, this.lexer.position, "{");
      }
      const usesStart = this.lexer.position;
      let uses = null;
      {
        const usesResult = this.parseUseList();
        if (usesResult.ok) uses = usesResult.value;
        else if (usesResult.error._tag === "InvalidSyntax") return usesResult;
      }
      const methodsStart = this.lexer.position;
      let methods = null;
      {
        const methodsResult = this.parseMethodList();
        if (methodsResult.ok) methods = methodsResult.value;
        else if (methodsResult.error._tag === "InvalidSyntax") return methodsResult;
      }
      const start_right_curly_8 = this.lexer.previousPosition;
      const success_right_curly_8 = this.lexer.parsePunctuation("}");
      if (!success_right_curly_8) {
        return invalid(start_right_curly_8, this.lexer.position, "}");
      }
      const start_colon_9 = this.lexer.previousPosition;
      const success_colon_9 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "ImplementSyntax",
        start,
        end,
        type,
        attributes,
        uses,
        methods
      });
    }
    parseTrait() {
      const start = this.lexer.previousPosition;
      const start_trait_1 = this.lexer.previousPosition;
      const success_trait_1 = this.lexer.parseKeyword("trait");
      if (!success_trait_1) return different();
      const nameStart = this.lexer.position;
      const nameResult = this.parseName();
      if (!nameResult.ok) {
        if (nameResult.error._tag === "InvalidSyntax") return nameResult;
        return invalid(nameStart, this.lexer.position, "a valid Name syntax");
      }
      const name = nameResult.value;
      const extensionStart = this.lexer.position;
      let extension = null;
      {
        const extensionResult = this.parseExtends();
        if (extensionResult.ok) extension = extensionResult.value;
        else if (extensionResult.error._tag === "InvalidSyntax") return extensionResult;
      }
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const start_left_curly_5 = this.lexer.previousPosition;
      const success_left_curly_5 = this.lexer.parsePunctuation("{");
      if (!success_left_curly_5) {
        return invalid(start_left_curly_5, this.lexer.position, "{");
      }
      const usesStart = this.lexer.position;
      let uses = null;
      {
        const usesResult = this.parseUseList();
        if (usesResult.ok) uses = usesResult.value;
        else if (usesResult.error._tag === "InvalidSyntax") return usesResult;
      }
      const functionsStart = this.lexer.position;
      let functions = null;
      {
        const functionsResult = this.parseMethodList();
        if (functionsResult.ok) functions = functionsResult.value;
        else if (functionsResult.error._tag === "InvalidSyntax") return functionsResult;
      }
      const start_right_curly_8 = this.lexer.previousPosition;
      const success_right_curly_8 = this.lexer.parsePunctuation("}");
      if (!success_right_curly_8) {
        return invalid(start_right_curly_8, this.lexer.position, "}");
      }
      const start_colon_9 = this.lexer.previousPosition;
      const success_colon_9 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "TraitSyntax",
        start,
        end,
        name,
        extension,
        attributes,
        uses,
        functions
      });
    }
    parseMethodList() {
      const list = [];
      while (true) {
        const result = this.parseMethod();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseMethod() {
      {
        const result = this.parseFunction();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseProcedure();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseOperator();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseInitList() {
      const list = [];
      while (true) {
        const result = this.parseInit();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseInit() {
      const start = this.lexer.previousPosition;
      const start_init_1 = this.lexer.previousPosition;
      const success_init_1 = this.lexer.parseKeyword("init");
      if (!success_init_1) return different();
      const parametersStart = this.lexer.position;
      let parameters = null;
      {
        const parametersResult = this.parseParameterSet();
        if (parametersResult.ok) parameters = parametersResult.value;
        else if (parametersResult.error._tag === "InvalidSyntax") return parametersResult;
      }
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const actionStart = this.lexer.position;
      const actionResult = this.parseAction();
      if (!actionResult.ok) {
        if (actionResult.error._tag === "InvalidSyntax") return actionResult;
        return invalid(actionStart, this.lexer.position, "a valid Action syntax");
      }
      const action = actionResult.value;
      const start_colon_5 = this.lexer.previousPosition;
      const success_colon_5 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "InitSyntax",
        start,
        end,
        parameters,
        action
      });
    }
    parseDeInit() {
      const start = this.lexer.previousPosition;
      const start_deinit_1 = this.lexer.previousPosition;
      const success_deinit_1 = this.lexer.parseKeyword("deinit");
      if (!success_deinit_1) return different();
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const actionStart = this.lexer.position;
      const actionResult = this.parseAction();
      if (!actionResult.ok) {
        if (actionResult.error._tag === "InvalidSyntax") return actionResult;
        return invalid(actionStart, this.lexer.position, "a valid Action syntax");
      }
      const action = actionResult.value;
      const start_colon_4 = this.lexer.previousPosition;
      const success_colon_4 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "DeInitSyntax",
        start,
        end,
        action
      });
    }
    parseFunction() {
      const start = this.lexer.previousPosition;
      const start_function_1 = this.lexer.previousPosition;
      const success_function_1 = this.lexer.parseKeyword("function");
      if (!success_function_1) return different();
      const targetStart = this.lexer.position;
      const targetResult = this.parseTarget();
      if (!targetResult.ok) {
        if (targetResult.error._tag === "InvalidSyntax") return targetResult;
        return invalid(targetStart, this.lexer.position, "a valid Target syntax");
      }
      const target = targetResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "FunctionSyntax",
        start,
        end,
        target
      });
    }
    parseProcedure() {
      const start = this.lexer.previousPosition;
      const start_procedure_1 = this.lexer.previousPosition;
      const success_procedure_1 = this.lexer.parseKeyword("procedure");
      if (!success_procedure_1) return different();
      const targetStart = this.lexer.position;
      const targetResult = this.parseTarget();
      if (!targetResult.ok) {
        if (targetResult.error._tag === "InvalidSyntax") return targetResult;
        return invalid(targetStart, this.lexer.position, "a valid Target syntax");
      }
      const target = targetResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "ProcedureSyntax",
        start,
        end,
        target
      });
    }
    parseOperator() {
      const start = this.lexer.previousPosition;
      const start_operator_1 = this.lexer.previousPosition;
      const success_operator_1 = this.lexer.parseKeyword("operator");
      if (!success_operator_1) return different();
      const targetStart = this.lexer.position;
      const targetResult = this.parseTarget();
      if (!targetResult.ok) {
        if (targetResult.error._tag === "InvalidSyntax") return targetResult;
        return invalid(targetStart, this.lexer.position, "a valid Target syntax");
      }
      const target = targetResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "OperatorSyntax",
        start,
        end,
        target
      });
    }
    parseTarget() {
      {
        const result = this.parseNamed();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseRoutine();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseNamed() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) return different();
      const routineStart = this.lexer.position;
      const routineResult = this.parseRoutine();
      if (!routineResult.ok) {
        if (routineResult.error._tag === "InvalidSyntax") return routineResult;
        return invalid(routineStart, this.lexer.position, "a valid Routine syntax");
      }
      const routine = routineResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "NamedSyntax",
        start,
        end,
        name,
        routine
      });
    }
    parseRoutine() {
      const start = this.lexer.previousPosition;
      const genericsStart = this.lexer.position;
      let generics = null;
      {
        const genericsResult = this.parseGenericArguments();
        if (genericsResult.ok) generics = genericsResult.value;
        else if (genericsResult.error._tag === "InvalidSyntax") return genericsResult;
      }
      const lifetimeStart = this.lexer.position;
      let lifetime = null;
      {
        const lifetimeResult = this.parseLifetime();
        if (lifetimeResult.ok) lifetime = lifetimeResult.value;
        else if (lifetimeResult.error._tag === "InvalidSyntax") return lifetimeResult;
      }
      const parametersStart = this.lexer.position;
      let parameters = null;
      {
        const parametersResult = this.parseParameterSet();
        if (parametersResult.ok) parameters = parametersResult.value;
        else if (parametersResult.error._tag === "InvalidSyntax") return parametersResult;
      }
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const returns_Start = this.lexer.position;
      let returns_ = null;
      {
        const returns_Result = this.parseReturns();
        if (returns_Result.ok) returns_ = returns_Result.value;
        else if (returns_Result.error._tag === "InvalidSyntax") return returns_Result;
      }
      const start_colon_6 = this.lexer.previousPosition;
      const success_colon_6 = this.lexer.parseColon();
      const throws_Start = this.lexer.position;
      let throws_ = null;
      {
        const throws_Result = this.parseThrows();
        if (throws_Result.ok) throws_ = throws_Result.value;
        else if (throws_Result.error._tag === "InvalidSyntax") return throws_Result;
      }
      const start_colon_8 = this.lexer.previousPosition;
      const success_colon_8 = this.lexer.parseColon();
      const implementationStart = this.lexer.position;
      const implementationResult = this.parseImplementation();
      if (!implementationResult.ok) {
        if (implementationResult.error._tag === "InvalidSyntax") return implementationResult;
        return invalid(implementationStart, this.lexer.position, "a valid Implementation syntax");
      }
      const implementation = implementationResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "RoutineSyntax",
        start,
        end,
        generics,
        lifetime,
        parameters,
        attributes,
        returns_,
        throws_,
        implementation
      });
    }
    parseImplementation() {
      {
        const result = this.parseAction();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseExtern();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseInstruction();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseIntrinsic();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseExtern() {
      const start = this.lexer.previousPosition;
      const start_extern_1 = this.lexer.previousPosition;
      const success_extern_1 = this.lexer.parseKeyword("extern");
      if (!success_extern_1) return different();
      const end = this.lexer.position;
      return ok({
        _syntax: "ExternSyntax",
        start,
        end
      });
    }
    parseInstruction() {
      const start = this.lexer.previousPosition;
      const start_instruction_1 = this.lexer.previousPosition;
      const success_instruction_1 = this.lexer.parseKeyword("instruction");
      if (!success_instruction_1) return different();
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "InstructionSyntax",
        start,
        end
      });
    }
    parseIntrinsic() {
      const start = this.lexer.previousPosition;
      const start_intrinsic_1 = this.lexer.previousPosition;
      const success_intrinsic_1 = this.lexer.parseKeyword("intrinsic");
      if (!success_intrinsic_1) return different();
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "IntrinsicSyntax",
        start,
        end
      });
    }
    parseExtends() {
      const start = this.lexer.previousPosition;
      const start_extends_1 = this.lexer.previousPosition;
      const success_extends_1 = this.lexer.parseKeyword("extends");
      if (!success_extends_1) return different();
      const extensionsStart = this.lexer.position;
      let extensions = null;
      {
        const extensionsResult = this.parseExtendList();
        if (extensionsResult.ok) extensions = extensionsResult.value;
        else if (extensionsResult.error._tag === "InvalidSyntax") return extensionsResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ExtendsSyntax",
        start,
        end,
        extensions
      });
    }
    parseExtendList() {
      const list = [];
      while (true) {
        const result = this.parseExtend();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseExtend() {
      const start = this.lexer.previousPosition;
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        return typeResult;
      }
      const type = typeResult.value;
      const start_comma_2 = this.lexer.previousPosition;
      const success_comma_2 = this.lexer.parsePunctuation(",");
      const end = this.lexer.position;
      return ok({
        _syntax: "ExtendSyntax",
        start,
        end,
        type
      });
    }
    parseMacro() {
      const start = this.lexer.previousPosition;
      const start_macro_1 = this.lexer.previousPosition;
      const success_macro_1 = this.lexer.parseKeyword("macro");
      if (!success_macro_1) return different();
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) {
        return invalid(start_name, this.lexer.position, "an identifier");
      }
      const modelStart = this.lexer.position;
      const modelResult = this.parseModel();
      if (!modelResult.ok) {
        if (modelResult.error._tag === "InvalidSyntax") return modelResult;
        return invalid(modelStart, this.lexer.position, "a valid Model syntax");
      }
      const model = modelResult.value;
      const ruleStart = this.lexer.position;
      const ruleResult = this.parseOperandList();
      if (!ruleResult.ok) {
        if (ruleResult.error._tag === "InvalidSyntax") return ruleResult;
        return invalid(ruleStart, this.lexer.position, "a valid Operand syntax");
      }
      const rule = ruleResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "MacroSyntax",
        start,
        end,
        name,
        model,
        rule
      });
    }
    parseAttributeList() {
      const list = [];
      while (true) {
        const result = this.parseAttribute();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseAttribute() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseAttribute();
      if (!name) return different();
      const modelStart = this.lexer.position;
      const modelResult = this.parseModel();
      if (!modelResult.ok) {
        if (modelResult.error._tag === "InvalidSyntax") return modelResult;
        return invalid(modelStart, this.lexer.position, "a valid Model syntax");
      }
      const model = modelResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "AttributeSyntax",
        start,
        end,
        name,
        model
      });
    }
    parseModel() {
      {
        const result = this.parseLiteral();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseName();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseObject();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseVector();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseModule() {
      const start = this.lexer.previousPosition;
      const start_module_1 = this.lexer.previousPosition;
      const success_module_1 = this.lexer.parseKeyword("module");
      if (!success_module_1) return different();
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) {
        return invalid(start_name, this.lexer.position, "an identifier");
      }
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "ModuleSyntax",
        start,
        end,
        name
      });
    }
    parsePackageList() {
      const list = [];
      while (true) {
        const result = this.parsePackage();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parsePackage() {
      const start = this.lexer.previousPosition;
      const start_package_1 = this.lexer.previousPosition;
      const success_package_1 = this.lexer.parseKeyword("package");
      if (!success_package_1) return different();
      const nameStart = this.lexer.position;
      const nameResult = this.parseName();
      if (!nameResult.ok) {
        if (nameResult.error._tag === "InvalidSyntax") return nameResult;
        return invalid(nameStart, this.lexer.position, "a valid Name syntax");
      }
      const name = nameResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "PackageSyntax",
        start,
        end,
        name
      });
    }
    parseInitializer() {
      const start = this.lexer.previousPosition;
      const start_left_paren_1 = this.lexer.previousPosition;
      const success_left_paren_1 = this.lexer.parsePunctuation("(");
      if (!success_left_paren_1) return different();
      const operandsStart = this.lexer.position;
      const operandsResult = this.parseOperandList();
      if (!operandsResult.ok) {
        if (operandsResult.error._tag === "InvalidSyntax") return operandsResult;
        return invalid(operandsStart, this.lexer.position, "a valid Operand syntax");
      }
      const operands = operandsResult.value;
      const start_right_paren_3 = this.lexer.previousPosition;
      const success_right_paren_3 = this.lexer.parsePunctuation(")");
      if (!success_right_paren_3) {
        return invalid(start_right_paren_3, this.lexer.position, ")");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "InitializerSyntax",
        start,
        end,
        operands
      });
    }
    parseOperandList() {
      const list = [];
      while (true) {
        const result = this.parseOperand();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseOperand() {
      const start = this.lexer.previousPosition;
      const expressionStart = this.lexer.position;
      const expressionResult = this.parseExpression();
      if (!expressionResult.ok) {
        return expressionResult;
      }
      const expression = expressionResult.value;
      const membersStart = this.lexer.position;
      let members = null;
      {
        const membersResult = this.parseMemberAccessList();
        if (membersResult.ok) members = membersResult.value;
        else if (membersResult.error._tag === "InvalidSyntax") return membersResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "OperandSyntax",
        start,
        end,
        expression,
        members
      });
    }
    parseMemberAccessList() {
      const list = [];
      while (true) {
        const result = this.parseMemberAccess();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseMemberAccess() {
      const start = this.lexer.previousPosition;
      const start_dot_1 = this.lexer.previousPosition;
      const success_dot_1 = this.lexer.parsePunctuation(".");
      if (!success_dot_1) return different();
      const nameStart = this.lexer.position;
      const nameResult = this.parseName();
      if (!nameResult.ok) {
        if (nameResult.error._tag === "InvalidSyntax") return nameResult;
        return invalid(nameStart, this.lexer.position, "a valid Name syntax");
      }
      const name = nameResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "MemberAccessSyntax",
        start,
        end,
        name
      });
    }
    parseExpression() {
      {
        const result = this.parseLiteral();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseType();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseObject();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseVector();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseBlock();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseIf();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseMatch();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseLambda();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseFor();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseWhile();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseChoose();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseTry();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseRepeat();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseSizeOf();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseIs();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseLiteral() {
      const start = this.lexer.previousPosition;
      const literalResult = this.parseLiteralToken();
      if (!literalResult.ok) return literalResult;
      const literal = literalResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "LiteralSyntax",
        start,
        end,
        literal
      });
    }
    parseObject() {
      const start = this.lexer.previousPosition;
      const start_left_paren_1 = this.lexer.previousPosition;
      const success_left_paren_1 = this.lexer.parsePunctuation("(");
      if (!success_left_paren_1) return different();
      const componentsStart = this.lexer.position;
      let components = null;
      {
        const componentsResult = this.parseComponentList();
        if (componentsResult.ok) components = componentsResult.value;
        else if (componentsResult.error._tag === "InvalidSyntax") return componentsResult;
      }
      const start_right_paren_3 = this.lexer.previousPosition;
      const success_right_paren_3 = this.lexer.parsePunctuation(")");
      if (!success_right_paren_3) {
        return invalid(start_right_paren_3, this.lexer.position, ")");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ObjectSyntax",
        start,
        end,
        components
      });
    }
    parseComponentList() {
      const list = [];
      while (true) {
        const result = this.parseComponent();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseComponent() {
      const start = this.lexer.previousPosition;
      const operandsStart = this.lexer.position;
      const operandsResult = this.parseOperandList();
      if (!operandsResult.ok) {
        return operandsResult;
      }
      const operands = operandsResult.value;
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const valueStart = this.lexer.position;
      let value = null;
      {
        const valueResult = this.parseValue();
        if (valueResult.ok) value = valueResult.value;
        else if (valueResult.error._tag === "InvalidSyntax") return valueResult;
      }
      const start_comma_4 = this.lexer.previousPosition;
      const success_comma_4 = this.lexer.parsePunctuation(",");
      const end = this.lexer.position;
      return ok({
        _syntax: "ComponentSyntax",
        start,
        end,
        operands,
        attributes,
        value
      });
    }
    parseValue() {
      const start = this.lexer.previousPosition;
      const start_colon_1 = this.lexer.previousPosition;
      const success_colon_1 = this.lexer.parseColon();
      if (!success_colon_1) return different();
      const valueStart = this.lexer.position;
      const valueResult = this.parseOperandList();
      if (!valueResult.ok) {
        if (valueResult.error._tag === "InvalidSyntax") return valueResult;
        return invalid(valueStart, this.lexer.position, "a valid Operand syntax");
      }
      const value = valueResult.value;
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ValueSyntax",
        start,
        end,
        value,
        attributes
      });
    }
    parseVector() {
      const start = this.lexer.previousPosition;
      const start_left_bracket_1 = this.lexer.previousPosition;
      const success_left_bracket_1 = this.lexer.parsePunctuation("[");
      if (!success_left_bracket_1) return different();
      const elementsStart = this.lexer.position;
      const elementsResult = this.parseElementList();
      if (!elementsResult.ok) {
        if (elementsResult.error._tag === "InvalidSyntax") return elementsResult;
        return invalid(elementsStart, this.lexer.position, "a valid Element syntax");
      }
      const elements = elementsResult.value;
      const start_right_bracket_3 = this.lexer.previousPosition;
      const success_right_bracket_3 = this.lexer.parsePunctuation("]");
      if (!success_right_bracket_3) {
        return invalid(start_right_bracket_3, this.lexer.position, "]");
      }
      const lifetimeStart = this.lexer.position;
      let lifetime = null;
      {
        const lifetimeResult = this.parseLifetime();
        if (lifetimeResult.ok) lifetime = lifetimeResult.value;
        else if (lifetimeResult.error._tag === "InvalidSyntax") return lifetimeResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "VectorSyntax",
        start,
        end,
        elements,
        lifetime
      });
    }
    parseElementList() {
      const list = [];
      while (true) {
        const result = this.parseElement();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseElement() {
      const start = this.lexer.previousPosition;
      const operationStart = this.lexer.position;
      const operationResult = this.parseOperandList();
      if (!operationResult.ok) {
        return operationResult;
      }
      const operation = operationResult.value;
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const start_comma_3 = this.lexer.previousPosition;
      const success_comma_3 = this.lexer.parsePunctuation(",");
      const end = this.lexer.position;
      return ok({
        _syntax: "ElementSyntax",
        start,
        end,
        operation,
        attributes
      });
    }
    parseBlock() {
      const start = this.lexer.previousPosition;
      const start_left_curly_1 = this.lexer.previousPosition;
      const success_left_curly_1 = this.lexer.parsePunctuation("{");
      if (!success_left_curly_1) return different();
      const usesStart = this.lexer.position;
      let uses = null;
      {
        const usesResult = this.parseUseList();
        if (usesResult.ok) uses = usesResult.value;
        else if (usesResult.error._tag === "InvalidSyntax") return usesResult;
      }
      const statementsStart = this.lexer.position;
      let statements = null;
      {
        const statementsResult = this.parseStatementList();
        if (statementsResult.ok) statements = statementsResult.value;
        else if (statementsResult.error._tag === "InvalidSyntax") return statementsResult;
      }
      const start_right_curly_4 = this.lexer.previousPosition;
      const success_right_curly_4 = this.lexer.parsePunctuation("}");
      if (!success_right_curly_4) {
        return invalid(start_right_curly_4, this.lexer.position, "}");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "BlockSyntax",
        start,
        end,
        uses,
        statements
      });
    }
    parseIf() {
      const start = this.lexer.previousPosition;
      const start_if_1 = this.lexer.previousPosition;
      const success_if_1 = this.lexer.parseKeyword("if");
      if (!success_if_1) return different();
      const conditionStart = this.lexer.position;
      const conditionResult = this.parseOperandList();
      if (!conditionResult.ok) {
        if (conditionResult.error._tag === "InvalidSyntax") return conditionResult;
        return invalid(conditionStart, this.lexer.position, "a valid Operand syntax");
      }
      const condition = conditionResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const consequentStart = this.lexer.position;
      const consequentResult = this.parseCommand();
      if (!consequentResult.ok) {
        if (consequentResult.error._tag === "InvalidSyntax") return consequentResult;
        return invalid(consequentStart, this.lexer.position, "a valid Command syntax");
      }
      const consequent = consequentResult.value;
      const alternativeStart = this.lexer.position;
      let alternative = null;
      {
        const alternativeResult = this.parseElse();
        if (alternativeResult.ok) alternative = alternativeResult.value;
        else if (alternativeResult.error._tag === "InvalidSyntax") return alternativeResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "IfSyntax",
        start,
        end,
        condition,
        consequent,
        alternative
      });
    }
    parseElse() {
      const start = this.lexer.previousPosition;
      const start_else_1 = this.lexer.previousPosition;
      const success_else_1 = this.lexer.parseKeyword("else");
      if (!success_else_1) return different();
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const alternativeStart = this.lexer.position;
      const alternativeResult = this.parseCommand();
      if (!alternativeResult.ok) {
        if (alternativeResult.error._tag === "InvalidSyntax") return alternativeResult;
        return invalid(alternativeStart, this.lexer.position, "a valid Command syntax");
      }
      const alternative = alternativeResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "ElseSyntax",
        start,
        end,
        alternative
      });
    }
    parseMatch() {
      const start = this.lexer.previousPosition;
      const start_match_1 = this.lexer.previousPosition;
      const success_match_1 = this.lexer.parseKeyword("match");
      if (!success_match_1) return different();
      const scrutineeStart = this.lexer.position;
      const scrutineeResult = this.parseOperandList();
      if (!scrutineeResult.ok) {
        if (scrutineeResult.error._tag === "InvalidSyntax") return scrutineeResult;
        return invalid(scrutineeStart, this.lexer.position, "a valid Operand syntax");
      }
      const scrutinee = scrutineeResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const branchesStart = this.lexer.position;
      const branchesResult = this.parseBranchList();
      if (!branchesResult.ok) {
        if (branchesResult.error._tag === "InvalidSyntax") return branchesResult;
        return invalid(branchesStart, this.lexer.position, "a valid Branch syntax");
      }
      const branches = branchesResult.value;
      const alternativeStart = this.lexer.position;
      let alternative = null;
      {
        const alternativeResult = this.parseElse();
        if (alternativeResult.ok) alternative = alternativeResult.value;
        else if (alternativeResult.error._tag === "InvalidSyntax") return alternativeResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "MatchSyntax",
        start,
        end,
        scrutinee,
        branches,
        alternative
      });
    }
    parseBranchList() {
      const list = [];
      while (true) {
        const result = this.parseBranch();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseBranch() {
      const start = this.lexer.previousPosition;
      const casesStart = this.lexer.position;
      const casesResult = this.parseCaseList();
      if (!casesResult.ok) {
        return casesResult;
      }
      const cases = casesResult.value;
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      if (!success_colon_2) {
        return invalid(start_colon_2, this.lexer.position, "a colon or a line feed");
      }
      const consequentStart = this.lexer.position;
      const consequentResult = this.parseStatement();
      if (!consequentResult.ok) {
        if (consequentResult.error._tag === "InvalidSyntax") return consequentResult;
        return invalid(consequentStart, this.lexer.position, "a valid Statement syntax");
      }
      const consequent = consequentResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "BranchSyntax",
        start,
        end,
        cases,
        consequent
      });
    }
    parseCaseList() {
      const list = [];
      while (true) {
        const result = this.parseCase();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseCase() {
      const start = this.lexer.previousPosition;
      const start_case_1 = this.lexer.previousPosition;
      const success_case_1 = this.lexer.parseKeyword("case");
      if (!success_case_1) return different();
      const conditionStart = this.lexer.position;
      const conditionResult = this.parseOperandList();
      if (!conditionResult.ok) {
        if (conditionResult.error._tag === "InvalidSyntax") return conditionResult;
        return invalid(conditionStart, this.lexer.position, "a valid Operand syntax");
      }
      const condition = conditionResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "CaseSyntax",
        start,
        end,
        condition
      });
    }
    parseStatementList() {
      const list = [];
      while (true) {
        const result = this.parseStatement();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseStatement() {
      const start = this.lexer.previousPosition;
      const commandStart = this.lexer.position;
      const commandResult = this.parseCommand();
      if (!commandResult.ok) {
        return commandResult;
      }
      const command = commandResult.value;
      const start_colon_2 = this.lexer.previousPosition;
      const success_colon_2 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "StatementSyntax",
        start,
        end,
        command
      });
    }
    parseWhile() {
      const start = this.lexer.previousPosition;
      const start_while_1 = this.lexer.previousPosition;
      const success_while_1 = this.lexer.parseKeyword("while");
      if (!success_while_1) return different();
      const conditionStart = this.lexer.position;
      const conditionResult = this.parseCondition();
      if (!conditionResult.ok) {
        if (conditionResult.error._tag === "InvalidSyntax") return conditionResult;
        return invalid(conditionStart, this.lexer.position, "a valid Condition syntax");
      }
      const condition = conditionResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const nameStart = this.lexer.position;
      let name = null;
      {
        const nameResult = this.parseLabel();
        if (nameResult.ok) name = nameResult.value;
        else if (nameResult.error._tag === "InvalidSyntax") return nameResult;
      }
      const actionStart = this.lexer.position;
      const actionResult = this.parseAction();
      if (!actionResult.ok) {
        if (actionResult.error._tag === "InvalidSyntax") return actionResult;
        return invalid(actionStart, this.lexer.position, "a valid Action syntax");
      }
      const action = actionResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "WhileSyntax",
        start,
        end,
        condition,
        name,
        action
      });
    }
    parseChoose() {
      const start = this.lexer.previousPosition;
      const start_choose_1 = this.lexer.previousPosition;
      const success_choose_1 = this.lexer.parseKeyword("choose");
      if (!success_choose_1) return different();
      const conditionStart = this.lexer.position;
      const conditionResult = this.parseOperandList();
      if (!conditionResult.ok) {
        if (conditionResult.error._tag === "InvalidSyntax") return conditionResult;
        return invalid(conditionStart, this.lexer.position, "a valid Operand syntax");
      }
      const condition = conditionResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const casesStart = this.lexer.position;
      let cases = null;
      {
        const casesResult = this.parseWhenList();
        if (casesResult.ok) cases = casesResult.value;
        else if (casesResult.error._tag === "InvalidSyntax") return casesResult;
      }
      const alternativeStart = this.lexer.position;
      let alternative = null;
      {
        const alternativeResult = this.parseElse();
        if (alternativeResult.ok) alternative = alternativeResult.value;
        else if (alternativeResult.error._tag === "InvalidSyntax") return alternativeResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ChooseSyntax",
        start,
        end,
        condition,
        cases,
        alternative
      });
    }
    parseTry() {
      const start = this.lexer.previousPosition;
      const start_try_1 = this.lexer.previousPosition;
      const success_try_1 = this.lexer.parseKeyword("try");
      if (!success_try_1) return different();
      const conditionStart = this.lexer.position;
      const conditionResult = this.parseCondition();
      if (!conditionResult.ok) {
        if (conditionResult.error._tag === "InvalidSyntax") return conditionResult;
        return invalid(conditionStart, this.lexer.position, "a valid Condition syntax");
      }
      const condition = conditionResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const casesStart = this.lexer.position;
      let cases = null;
      {
        const casesResult = this.parseWhenList();
        if (casesResult.ok) cases = casesResult.value;
        else if (casesResult.error._tag === "InvalidSyntax") return casesResult;
      }
      const dropperStart = this.lexer.position;
      let dropper = null;
      {
        const dropperResult = this.parseElse();
        if (dropperResult.ok) dropper = dropperResult.value;
        else if (dropperResult.error._tag === "InvalidSyntax") return dropperResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "TrySyntax",
        start,
        end,
        condition,
        cases,
        dropper
      });
    }
    parseCondition() {
      {
        const result = this.parseOperation();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseLet();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseWhenList() {
      const list = [];
      while (true) {
        const result = this.parseWhen();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseWhen() {
      const start = this.lexer.previousPosition;
      const start_when_1 = this.lexer.previousPosition;
      const success_when_1 = this.lexer.parseKeyword("when");
      if (!success_when_1) return different();
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) {
        return invalid(start_name, this.lexer.position, "an identifier");
      }
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const variantStart = this.lexer.position;
      const variantResult = this.parseName();
      if (!variantResult.ok) {
        if (variantResult.error._tag === "InvalidSyntax") return variantResult;
        return invalid(variantStart, this.lexer.position, "a valid Name syntax");
      }
      const variant = variantResult.value;
      const start_colon_5 = this.lexer.previousPosition;
      const success_colon_5 = this.lexer.parseColon();
      const commandStart = this.lexer.position;
      const commandResult = this.parseCommand();
      if (!commandResult.ok) {
        if (commandResult.error._tag === "InvalidSyntax") return commandResult;
        return invalid(commandStart, this.lexer.position, "a valid Command syntax");
      }
      const command = commandResult.value;
      const start_colon_7 = this.lexer.previousPosition;
      const success_colon_7 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "WhenSyntax",
        start,
        end,
        name,
        variant,
        command
      });
    }
    parseCommand() {
      {
        const result = this.parseOperation();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseLet();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseVar();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseMutable();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseSet();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseContinue();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseBreak();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseReturn();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseThrow();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseLet() {
      const start = this.lexer.previousPosition;
      const start_let_1 = this.lexer.previousPosition;
      const success_let_1 = this.lexer.parseKeyword("let");
      if (!success_let_1) return different();
      const bindingStart = this.lexer.position;
      const bindingResult = this.parseBinding();
      if (!bindingResult.ok) {
        if (bindingResult.error._tag === "InvalidSyntax") return bindingResult;
        return invalid(bindingStart, this.lexer.position, "a valid Binding syntax");
      }
      const binding = bindingResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "LetSyntax",
        start,
        end,
        binding
      });
    }
    parseVar() {
      const start = this.lexer.previousPosition;
      const start_var_1 = this.lexer.previousPosition;
      const success_var_1 = this.lexer.parseKeyword("var");
      if (!success_var_1) return different();
      const bindingStart = this.lexer.position;
      const bindingResult = this.parseBinding();
      if (!bindingResult.ok) {
        if (bindingResult.error._tag === "InvalidSyntax") return bindingResult;
        return invalid(bindingStart, this.lexer.position, "a valid Binding syntax");
      }
      const binding = bindingResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "VarSyntax",
        start,
        end,
        binding
      });
    }
    parseMutable() {
      const start = this.lexer.previousPosition;
      const start_mutable_1 = this.lexer.previousPosition;
      const success_mutable_1 = this.lexer.parseKeyword("mutable");
      if (!success_mutable_1) return different();
      const bindingStart = this.lexer.position;
      const bindingResult = this.parseBinding();
      if (!bindingResult.ok) {
        if (bindingResult.error._tag === "InvalidSyntax") return bindingResult;
        return invalid(bindingStart, this.lexer.position, "a valid Binding syntax");
      }
      const binding = bindingResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "MutableSyntax",
        start,
        end,
        binding
      });
    }
    parseBinding() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) return different();
      const annotationStart = this.lexer.position;
      let annotation = null;
      {
        const annotationResult = this.parseBindingAnnotation();
        if (annotationResult.ok) annotation = annotationResult.value;
        else if (annotationResult.error._tag === "InvalidSyntax") return annotationResult;
      }
      const operationStart = this.lexer.position;
      const operationResult = this.parseOperandList();
      if (!operationResult.ok) {
        if (operationResult.error._tag === "InvalidSyntax") return operationResult;
        return invalid(operationStart, this.lexer.position, "a valid Operand syntax");
      }
      const operation = operationResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "BindingSyntax",
        start,
        end,
        name,
        annotation,
        operation
      });
    }
    parseBindingAnnotation() {
      const start = this.lexer.previousPosition;
      const start_colon_1 = this.lexer.previousPosition;
      const success_colon_1 = this.lexer.parseColon();
      if (!success_colon_1) return different();
      const specStart = this.lexer.position;
      const specResult = this.parseBindingSpec();
      if (!specResult.ok) {
        if (specResult.error._tag === "InvalidSyntax") return specResult;
        return invalid(specStart, this.lexer.position, "a valid BindingSpec syntax");
      }
      const spec = specResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "BindingAnnotationSyntax",
        start,
        end,
        spec
      });
    }
    parseBindingSpecList() {
      const list = [];
      while (true) {
        const result = this.parseBindingSpec();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseBindingSpec() {
      {
        const result = this.parseStructure();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseType();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseArray();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseArray() {
      const start = this.lexer.previousPosition;
      const start_left_bracket_1 = this.lexer.previousPosition;
      const success_left_bracket_1 = this.lexer.parsePunctuation("[");
      if (!success_left_bracket_1) return different();
      const membersStart = this.lexer.position;
      let members = null;
      {
        const membersResult = this.parseTypeList();
        if (membersResult.ok) members = membersResult.value;
        else if (membersResult.error._tag === "InvalidSyntax") return membersResult;
      }
      const start_right_bracket_3 = this.lexer.previousPosition;
      const success_right_bracket_3 = this.lexer.parsePunctuation("]");
      if (!success_right_bracket_3) {
        return invalid(start_right_bracket_3, this.lexer.position, "]");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ArraySyntax",
        start,
        end,
        members
      });
    }
    parseStructure() {
      const start = this.lexer.previousPosition;
      const start_left_paren_1 = this.lexer.previousPosition;
      const success_left_paren_1 = this.lexer.parsePunctuation("(");
      if (!success_left_paren_1) return different();
      const partsStart = this.lexer.position;
      let parts = null;
      {
        const partsResult = this.parsePartList();
        if (partsResult.ok) parts = partsResult.value;
        else if (partsResult.error._tag === "InvalidSyntax") return partsResult;
      }
      const start_right_paren_3 = this.lexer.previousPosition;
      const success_right_paren_3 = this.lexer.parsePunctuation(")");
      if (!success_right_paren_3) {
        return invalid(start_right_paren_3, this.lexer.position, ")");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "StructureSyntax",
        start,
        end,
        parts
      });
    }
    parsePartList() {
      const list = [];
      while (true) {
        const result = this.parsePart();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parsePart() {
      {
        const result = this.parseField();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseProperty();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseField() {
      const start = this.lexer.previousPosition;
      const start_private_1 = this.lexer.previousPosition;
      const success_private_1 = this.lexer.parseKeyword("private");
      if (!success_private_1) return different();
      const propertyStart = this.lexer.position;
      const propertyResult = this.parseProperty();
      if (!propertyResult.ok) {
        if (propertyResult.error._tag === "InvalidSyntax") return propertyResult;
        return invalid(propertyStart, this.lexer.position, "a valid Property syntax");
      }
      const property = propertyResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "FieldSyntax",
        start,
        end,
        property
      });
    }
    parsePropertyList() {
      const list = [];
      while (true) {
        const result = this.parseProperty();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseProperty() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) return different();
      const annotationStart = this.lexer.position;
      const annotationResult = this.parseTypeAnnotation();
      if (!annotationResult.ok) {
        if (annotationResult.error._tag === "InvalidSyntax") return annotationResult;
        return invalid(annotationStart, this.lexer.position, "a valid TypeAnnotation syntax");
      }
      const annotation = annotationResult.value;
      const initializerStart = this.lexer.position;
      let initializer = null;
      {
        const initializerResult = this.parseInitializer();
        if (initializerResult.ok) initializer = initializerResult.value;
        else if (initializerResult.error._tag === "InvalidSyntax") return initializerResult;
      }
      const start_comma_4 = this.lexer.previousPosition;
      const success_comma_4 = this.lexer.parsePunctuation(",");
      const start_colon_5 = this.lexer.previousPosition;
      const success_colon_5 = this.lexer.parseColon();
      const attributesStart = this.lexer.position;
      let attributes = null;
      {
        const attributesResult = this.parseAttributeList();
        if (attributesResult.ok) attributes = attributesResult.value;
        else if (attributesResult.error._tag === "InvalidSyntax") return attributesResult;
      }
      const start_colon_7 = this.lexer.previousPosition;
      const success_colon_7 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "PropertySyntax",
        start,
        end,
        name,
        annotation,
        initializer,
        attributes
      });
    }
    parseTypeAnnotation() {
      const start = this.lexer.previousPosition;
      const start_colon_1 = this.lexer.previousPosition;
      const success_colon_1 = this.lexer.parseColon();
      if (!success_colon_1) return different();
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        if (typeResult.error._tag === "InvalidSyntax") return typeResult;
        return invalid(typeStart, this.lexer.position, "a valid Type syntax");
      }
      const type = typeResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "TypeAnnotationSyntax",
        start,
        end,
        type
      });
    }
    parseContinue() {
      const start = this.lexer.previousPosition;
      const start_continue_1 = this.lexer.previousPosition;
      const success_continue_1 = this.lexer.parseKeyword("continue");
      if (!success_continue_1) return different();
      const nameStart = this.lexer.position;
      let name = null;
      {
        const nameResult = this.parseLoop();
        if (nameResult.ok) name = nameResult.value;
        else if (nameResult.error._tag === "InvalidSyntax") return nameResult;
      }
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "ContinueSyntax",
        start,
        end,
        name
      });
    }
    parseBreak() {
      const start = this.lexer.previousPosition;
      const start_break_1 = this.lexer.previousPosition;
      const success_break_1 = this.lexer.parseKeyword("break");
      if (!success_break_1) return different();
      const nameStart = this.lexer.position;
      let name = null;
      {
        const nameResult = this.parseLoop();
        if (nameResult.ok) name = nameResult.value;
        else if (nameResult.error._tag === "InvalidSyntax") return nameResult;
      }
      const resultStart = this.lexer.position;
      let result = null;
      {
        const resultResult = this.parseOperandList();
        if (resultResult.ok) result = resultResult.value;
        else if (resultResult.error._tag === "InvalidSyntax") return resultResult;
      }
      const start_colon_4 = this.lexer.previousPosition;
      const success_colon_4 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "BreakSyntax",
        start,
        end,
        name,
        result
      });
    }
    parseLoop() {
      const start = this.lexer.previousPosition;
      const start_loop_1 = this.lexer.previousPosition;
      const success_loop_1 = this.lexer.parseKeyword("loop");
      if (!success_loop_1) return different();
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) {
        return invalid(start_name, this.lexer.position, "an identifier");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "LoopSyntax",
        start,
        end,
        name
      });
    }
    parseReturn() {
      const start = this.lexer.previousPosition;
      const start_return_1 = this.lexer.previousPosition;
      const success_return_1 = this.lexer.parseKeyword("return");
      if (!success_return_1) return different();
      const resultStart = this.lexer.position;
      let result = null;
      {
        const resultResult = this.parseOperandList();
        if (resultResult.ok) result = resultResult.value;
        else if (resultResult.error._tag === "InvalidSyntax") return resultResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ReturnSyntax",
        start,
        end,
        result
      });
    }
    parseThrow() {
      const start = this.lexer.previousPosition;
      const start_throw_1 = this.lexer.previousPosition;
      const success_throw_1 = this.lexer.parseKeyword("throw");
      if (!success_throw_1) return different();
      const resultStart = this.lexer.position;
      let result = null;
      {
        const resultResult = this.parseOperandList();
        if (resultResult.ok) result = resultResult.value;
        else if (resultResult.error._tag === "InvalidSyntax") return resultResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ThrowSyntax",
        start,
        end,
        result
      });
    }
    parseLambda() {
      const start = this.lexer.previousPosition;
      const start_lambda_1 = this.lexer.previousPosition;
      const success_lambda_1 = this.lexer.parseKeyword("lambda");
      if (!success_lambda_1) return different();
      const inputStart = this.lexer.position;
      const inputResult = this.parseOperandList();
      if (!inputResult.ok) {
        if (inputResult.error._tag === "InvalidSyntax") return inputResult;
        return invalid(inputStart, this.lexer.position, "a valid Operand syntax");
      }
      const input = inputResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const blockStart = this.lexer.position;
      const blockResult = this.parseAction();
      if (!blockResult.ok) {
        if (blockResult.error._tag === "InvalidSyntax") return blockResult;
        return invalid(blockStart, this.lexer.position, "a valid Action syntax");
      }
      const block = blockResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "LambdaSyntax",
        start,
        end,
        input,
        block
      });
    }
    parseFor() {
      const start = this.lexer.previousPosition;
      const start_for_1 = this.lexer.previousPosition;
      const success_for_1 = this.lexer.parseKeyword("for");
      if (!success_for_1) return different();
      const start_variable = this.lexer.previousPosition;
      const variable = this.lexer.parseIdentifier(KEYWORDS);
      if (!variable) {
        return invalid(start_variable, this.lexer.position, "an identifier");
      }
      const annotationStart = this.lexer.position;
      let annotation = null;
      {
        const annotationResult = this.parseTypeAnnotation();
        if (annotationResult.ok) annotation = annotationResult.value;
        else if (annotationResult.error._tag === "InvalidSyntax") return annotationResult;
      }
      const start_in_4 = this.lexer.previousPosition;
      const success_in_4 = this.lexer.parseKeyword("in");
      if (!success_in_4) {
        return invalid(start_in_4, this.lexer.position, "in");
      }
      const operationStart = this.lexer.position;
      const operationResult = this.parseOperandList();
      if (!operationResult.ok) {
        if (operationResult.error._tag === "InvalidSyntax") return operationResult;
        return invalid(operationStart, this.lexer.position, "a valid Operand syntax");
      }
      const operation = operationResult.value;
      const start_colon_6 = this.lexer.previousPosition;
      const success_colon_6 = this.lexer.parseColon();
      if (!success_colon_6) {
        return invalid(start_colon_6, this.lexer.position, "a colon or a line feed");
      }
      const nameStart = this.lexer.position;
      let name = null;
      {
        const nameResult = this.parseLabel();
        if (nameResult.ok) name = nameResult.value;
        else if (nameResult.error._tag === "InvalidSyntax") return nameResult;
      }
      const actionStart = this.lexer.position;
      const actionResult = this.parseAction();
      if (!actionResult.ok) {
        if (actionResult.error._tag === "InvalidSyntax") return actionResult;
        return invalid(actionStart, this.lexer.position, "a valid Action syntax");
      }
      const action = actionResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "ForSyntax",
        start,
        end,
        variable,
        annotation,
        operation,
        name,
        action
      });
    }
    parseLabel() {
      const start = this.lexer.previousPosition;
      const start_label_1 = this.lexer.previousPosition;
      const success_label_1 = this.lexer.parseKeyword("label");
      if (!success_label_1) return different();
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) {
        return invalid(start_name, this.lexer.position, "an identifier");
      }
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      const end = this.lexer.position;
      return ok({
        _syntax: "LabelSyntax",
        start,
        end,
        name
      });
    }
    parseRepeat() {
      const start = this.lexer.previousPosition;
      const start_repeat_1 = this.lexer.previousPosition;
      const success_repeat_1 = this.lexer.parseKeyword("repeat");
      if (!success_repeat_1) return different();
      const nameStart = this.lexer.position;
      let name = null;
      {
        const nameResult = this.parseLabel();
        if (nameResult.ok) name = nameResult.value;
        else if (nameResult.error._tag === "InvalidSyntax") return nameResult;
      }
      const actionStart = this.lexer.position;
      const actionResult = this.parseAction();
      if (!actionResult.ok) {
        if (actionResult.error._tag === "InvalidSyntax") return actionResult;
        return invalid(actionStart, this.lexer.position, "a valid Action syntax");
      }
      const action = actionResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "RepeatSyntax",
        start,
        end,
        name,
        action
      });
    }
    parseActionList() {
      const list = [];
      while (true) {
        const result = this.parseAction();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseAction() {
      {
        const result = this.parseOperation();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseSet();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseOperation() {
      const start = this.lexer.previousPosition;
      const operandsStart = this.lexer.position;
      const operandsResult = this.parseOperandList();
      if (!operandsResult.ok) {
        return operandsResult;
      }
      const operands = operandsResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "OperationSyntax",
        start,
        end,
        operands
      });
    }
    parseSet() {
      const start = this.lexer.previousPosition;
      const start_set_1 = this.lexer.previousPosition;
      const success_set_1 = this.lexer.parseKeyword("set");
      if (!success_set_1) return different();
      const targetStart = this.lexer.position;
      const targetResult = this.parseOperandList();
      if (!targetResult.ok) {
        if (targetResult.error._tag === "InvalidSyntax") return targetResult;
        return invalid(targetStart, this.lexer.position, "a valid Operand syntax");
      }
      const target = targetResult.value;
      const start_colon_3 = this.lexer.previousPosition;
      const success_colon_3 = this.lexer.parseColon();
      if (!success_colon_3) {
        return invalid(start_colon_3, this.lexer.position, "a colon or a line feed");
      }
      const sourceStart = this.lexer.position;
      const sourceResult = this.parseOperandList();
      if (!sourceResult.ok) {
        if (sourceResult.error._tag === "InvalidSyntax") return sourceResult;
        return invalid(sourceStart, this.lexer.position, "a valid Operand syntax");
      }
      const source = sourceResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "SetSyntax",
        start,
        end,
        target,
        source
      });
    }
    parseSizeOf() {
      const start = this.lexer.previousPosition;
      const start_sizeof_1 = this.lexer.previousPosition;
      const success_sizeof_1 = this.lexer.parseKeyword("sizeof");
      if (!success_sizeof_1) return different();
      const typeStart = this.lexer.position;
      const typeResult = this.parseType();
      if (!typeResult.ok) {
        if (typeResult.error._tag === "InvalidSyntax") return typeResult;
        return invalid(typeStart, this.lexer.position, "a valid Type syntax");
      }
      const type = typeResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "SizeOfSyntax",
        start,
        end,
        type
      });
    }
    parseIs() {
      const start = this.lexer.previousPosition;
      const start_is_1 = this.lexer.previousPosition;
      const success_is_1 = this.lexer.parseKeyword("is");
      if (!success_is_1) return different();
      const nameStart = this.lexer.position;
      const nameResult = this.parseName();
      if (!nameResult.ok) {
        if (nameResult.error._tag === "InvalidSyntax") return nameResult;
        return invalid(nameStart, this.lexer.position, "a valid Name syntax");
      }
      const name = nameResult.value;
      const end = this.lexer.position;
      return ok({
        _syntax: "IsSyntax",
        start,
        end,
        name
      });
    }
    parseTypeList() {
      const list = [];
      while (true) {
        const result = this.parseType();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseType() {
      const start = this.lexer.previousPosition;
      const nameStart = this.lexer.position;
      const nameResult = this.parseName();
      if (!nameResult.ok) {
        return nameResult;
      }
      const name = nameResult.value;
      const genericsStart = this.lexer.position;
      let generics = null;
      {
        const genericsResult = this.parseGenericArguments();
        if (genericsResult.ok) generics = genericsResult.value;
        else if (genericsResult.error._tag === "InvalidSyntax") return genericsResult;
      }
      const optionalStart = this.lexer.position;
      let optional = null;
      {
        const optionalResult = this.parseOptional();
        if (optionalResult.ok) optional = optionalResult.value;
        else if (optionalResult.error._tag === "InvalidSyntax") return optionalResult;
      }
      const lifetimeStart = this.lexer.position;
      let lifetime = null;
      {
        const lifetimeResult = this.parseLifetime();
        if (lifetimeResult.ok) lifetime = lifetimeResult.value;
        else if (lifetimeResult.error._tag === "InvalidSyntax") return lifetimeResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "TypeSyntax",
        start,
        end,
        name,
        generics,
        optional,
        lifetime
      });
    }
    parseName() {
      const start = this.lexer.previousPosition;
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) return different();
      const extensionsStart = this.lexer.position;
      let extensions = null;
      {
        const extensionsResult = this.parseExtensionList();
        if (extensionsResult.ok) extensions = extensionsResult.value;
        else if (extensionsResult.error._tag === "InvalidSyntax") return extensionsResult;
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "NameSyntax",
        start,
        end,
        name,
        extensions
      });
    }
    parseExtensionList() {
      const list = [];
      while (true) {
        const result = this.parseExtension();
        if (!result.ok) {
          if (result.error._tag === "InvalidSyntax") return result;
          if (list.length === 0) return result;
          return ok(list);
        }
        list.push(result.value);
      }
    }
    parseExtension() {
      const start = this.lexer.previousPosition;
      const start_dot_1 = this.lexer.previousPosition;
      const success_dot_1 = this.lexer.parsePunctuation(".");
      if (!success_dot_1) return different();
      const start_name = this.lexer.previousPosition;
      const name = this.lexer.parseIdentifier(KEYWORDS);
      if (!name) {
        return invalid(start_name, this.lexer.position, "an identifier");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ExtensionSyntax",
        start,
        end,
        name
      });
    }
    parseLifetime() {
      {
        const result = this.parseCall();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseLocal();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseReference();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      {
        const result = this.parseThrown();
        if (result.ok) return ok(result.value);
        if (result.error._tag === "InvalidSyntax") return result;
      }
      return different();
    }
    parseCall() {
      const start = this.lexer.previousPosition;
      const start_hash_1 = this.lexer.previousPosition;
      const success_hash_1 = this.lexer.parsePunctuation("#");
      if (!success_hash_1) return different();
      const end = this.lexer.position;
      return ok({
        _syntax: "CallSyntax",
        start,
        end
      });
    }
    parseLocal() {
      const start = this.lexer.previousPosition;
      const start_dollar_1 = this.lexer.previousPosition;
      const success_dollar_1 = this.lexer.parsePunctuation("$");
      if (!success_dollar_1) return different();
      const end = this.lexer.position;
      return ok({
        _syntax: "LocalSyntax",
        start,
        end
      });
    }
    parseReference() {
      const start = this.lexer.previousPosition;
      const start_caret_1 = this.lexer.previousPosition;
      const success_caret_1 = this.lexer.parsePunctuation("^");
      if (!success_caret_1) return different();
      const start_location = this.lexer.previousPosition;
      const location = this.lexer.parseIdentifier(KEYWORDS);
      if (!location) {
        return invalid(start_location, this.lexer.position, "an identifier");
      }
      const end = this.lexer.position;
      return ok({
        _syntax: "ReferenceSyntax",
        start,
        end,
        location
      });
    }
    parseThrown() {
      const start = this.lexer.previousPosition;
      const start_exclamation_1 = this.lexer.previousPosition;
      const success_exclamation_1 = this.lexer.parsePunctuation("!");
      if (!success_exclamation_1) return different();
      const end = this.lexer.position;
      return ok({
        _syntax: "ThrownSyntax",
        start,
        end
      });
    }
    isAtEnd() {
      return this.lexer.isAtEnd();
    }
  };

  // src/modeler.ts
  function ok2(value) {
    return { ok: true, value };
  }
  function fail(error) {
    return { ok: false, error };
  }
  var Modeler = class {
    file;
    constructor(file = "<input>") {
      this.file = file;
    }
    // === Public API ===
    modelOperation(syntax) {
      return this.handleOperation(syntax);
    }
    modelExpression(syntax) {
      return this.handleExpression(syntax);
    }
    // === Literal Handling ===
    handleLiteral(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const literal = syntax.literal;
      switch (literal._syntax) {
        case "BooleanLiteral":
          return ok2({
            _tag: "BooleanConstant",
            span,
            value: literal.value
          });
        case "IntegerLiteral": {
          const value = parseInt(literal.value, 10);
          if (!Number.isFinite(value)) {
            return fail(invalidConstant(this.file, span));
          }
          return ok2({
            _tag: "IntegerConstant",
            span,
            value
          });
        }
        case "HexLiteral": {
          const value = parseInt(literal.value, 16);
          if (!Number.isFinite(value)) {
            return fail(invalidConstant(this.file, span));
          }
          return ok2({
            _tag: "HexConstant",
            span,
            value
          });
        }
        case "FloatingPointLiteral": {
          const value = parseFloat(literal.value);
          if (!Number.isFinite(value)) {
            return fail(invalidConstant(this.file, span));
          }
          return ok2({
            _tag: "FloatingPointConstant",
            span,
            value
          });
        }
        case "StringLiteral":
          return ok2({
            _tag: "StringConstant",
            span,
            value: literal.value
          });
        case "CharacterLiteral":
          return ok2({
            _tag: "CharacterConstant",
            span,
            value: literal.value
          });
        case "FragmentLiteral":
          return ok2({
            _tag: "FragmentConstant",
            span,
            value: literal.value
          });
      }
    }
    // === Type Handling ===
    handleType(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const path = [syntax.name.name];
      if (syntax.name.extensions) {
        for (const ext of syntax.name.extensions) {
          path.push(ext.name);
        }
      }
      let generics = null;
      if (syntax.generics) {
        generics = [];
        if (syntax.generics.generics) {
          for (const generic of syntax.generics.generics) {
            const typeResult = this.handleType(generic.type);
            if (!typeResult.ok) return typeResult;
            generics.push(typeResult.value);
          }
        }
      }
      let lifetime = { _tag: "Unspecified" };
      if (syntax.lifetime) {
        const lt = syntax.lifetime;
        switch (lt._syntax) {
          case "CallSyntax":
            lifetime = { _tag: "Call", span: { start: lt.start, end: lt.end } };
            break;
          case "LocalSyntax":
            lifetime = { _tag: "Local", span: { start: lt.start, end: lt.end } };
            break;
          case "ReferenceSyntax":
            lifetime = { _tag: "Reference", span: { start: lt.start, end: lt.end }, location: lt.location };
            break;
          case "ThrownSyntax":
            lifetime = { _tag: "Thrown", span: { start: lt.start, end: lt.end } };
            break;
        }
      }
      return ok2({
        _tag: "Type",
        span,
        name: path,
        generics,
        lifetime
      });
    }
    // === Expression Handling ===
    handleExpression(syntax) {
      switch (syntax._syntax) {
        case "LiteralSyntax":
          return this.handleLiteral(syntax);
        case "TypeSyntax":
          return this.handleType(syntax);
        case "ObjectSyntax":
          return this.handleObject(syntax);
        case "VectorSyntax":
          return this.handleVector(syntax);
        case "BlockSyntax":
          return this.handleBlock(syntax);
        case "IfSyntax":
          return this.handleIf(syntax);
        case "MatchSyntax":
          return this.handleMatch(syntax);
        case "ForSyntax":
          return this.handleFor(syntax);
        case "WhileSyntax":
          return this.handleWhile(syntax);
        case "ChooseSyntax":
          return this.handleChoose(syntax);
        case "TrySyntax":
          return this.handleTry(syntax);
        case "SizeOfSyntax":
          return this.handleSizeOf(syntax);
        case "IsSyntax":
          return this.handleIs(syntax);
        case "LambdaSyntax":
          return fail(notImplemented(this.file, "Lambda", { start: syntax.start, end: syntax.end }));
        case "RepeatSyntax":
          return fail(notImplemented(this.file, "Repeat", { start: syntax.start, end: syntax.end }));
      }
    }
    // === Object/Tuple Handling ===
    handleObject(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const components = [];
      if (syntax.components) {
        for (const comp of syntax.components) {
          const compResult = this.handleComponent(comp);
          if (!compResult.ok) return compResult;
          components.push(compResult.value);
        }
      }
      return ok2({
        _tag: "Tuple",
        span,
        components
      });
    }
    handleComponent(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const operandsResult = this.handleOperands(syntax.operands);
      if (!operandsResult.ok) return operandsResult;
      const name = null;
      const attributes = [];
      if (syntax.attributes) {
        for (const attr of syntax.attributes) {
          const attrResult = this.handleAttribute(attr);
          if (!attrResult.ok) return attrResult;
          attributes.push(attrResult.value);
        }
      }
      return ok2({
        span,
        name,
        value: operandsResult.value,
        attributes
      });
    }
    // === Vector/Matrix Handling ===
    handleVector(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const operations = [];
      for (const elem of syntax.elements) {
        const elemResult = this.handleOperands(elem.operation);
        if (!elemResult.ok) return elemResult;
        operations.push(elemResult.value);
      }
      return ok2({
        _tag: "Matrix",
        span,
        operations
      });
    }
    // === Block Handling ===
    handleBlock(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const statements = [];
      if (syntax.statements) {
        for (const stmt of syntax.statements) {
          const stmtResult = this.handleStatement(stmt);
          if (!stmtResult.ok) return stmtResult;
          statements.push(stmtResult.value);
        }
      }
      return ok2({
        _tag: "Block",
        span,
        statements
      });
    }
    // === Statement Handling ===
    handleStatement(syntax) {
      return this.handleCommand(syntax.command);
    }
    handleCommand(command) {
      switch (command._syntax) {
        case "OperationSyntax": {
          const operands = this.handleOperation(command);
          if (!operands.ok) return operands;
          return ok2({
            _tag: "Action",
            source: operands.value,
            target: []
          });
        }
        case "SetSyntax": {
          const source = this.handleOperands(command.source);
          if (!source.ok) return source;
          const target = this.handleOperands(command.target);
          if (!target.ok) return target;
          return ok2({
            _tag: "Action",
            source: source.value,
            target: target.value
          });
        }
        case "LetSyntax": {
          const itemResult = this.handleBindingItem(command.binding);
          if (!itemResult.ok) return itemResult;
          const operation = command.binding.operation ? this.handleOperands(command.binding.operation) : ok2([]);
          if (!operation.ok) return operation;
          return ok2({
            _tag: "Binding",
            span: { start: command.start, end: command.end },
            bindingType: "let",
            item: itemResult.value,
            operation: operation.value
          });
        }
        case "VarSyntax": {
          const itemResult = this.handleBindingItem(command.binding);
          if (!itemResult.ok) return itemResult;
          const operation = command.binding.operation ? this.handleOperands(command.binding.operation) : ok2([]);
          if (!operation.ok) return operation;
          return ok2({
            _tag: "Binding",
            span: { start: command.start, end: command.end },
            bindingType: "var",
            item: itemResult.value,
            operation: operation.value
          });
        }
        case "MutableSyntax": {
          const itemResult = this.handleBindingItem(command.binding);
          if (!itemResult.ok) return itemResult;
          const operation = command.binding.operation ? this.handleOperands(command.binding.operation) : ok2([]);
          if (!operation.ok) return operation;
          return ok2({
            _tag: "Binding",
            span: { start: command.start, end: command.end },
            bindingType: "mutable",
            item: itemResult.value,
            operation: operation.value
          });
        }
        case "BreakSyntax":
          return this.handleBreak(command);
        case "ContinueSyntax":
          return ok2({
            _tag: "Continue",
            span: { start: command.start, end: command.end }
          });
        case "ReturnSyntax":
          return this.handleReturn(command);
        case "ThrowSyntax":
          return this.handleThrow(command);
      }
    }
    handleBreak(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const result = syntax.result ? this.handleOperands(syntax.result) : ok2([]);
      if (!result.ok) return result;
      return ok2({
        _tag: "Break",
        span,
        result: result.value
      });
    }
    handleReturn(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const result = syntax.result ? this.handleOperands(syntax.result) : ok2([]);
      if (!result.ok) return result;
      return ok2({
        _tag: "Return",
        span,
        result: result.value
      });
    }
    handleThrow(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const result = syntax.result ? this.handleOperands(syntax.result) : ok2([]);
      if (!result.ok) return result;
      return ok2({
        _tag: "Throw",
        span,
        result: result.value
      });
    }
    // === Control Flow Handling ===
    handleIf(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const condition = this.handleOperands(syntax.condition);
      if (!condition.ok) return condition;
      const consequentResult = this.handleCommand(syntax.consequent);
      if (!consequentResult.ok) return consequentResult;
      let alternative = null;
      if (syntax.alternative) {
        const altResult = this.handleCommand(syntax.alternative.alternative);
        if (!altResult.ok) return altResult;
        alternative = altResult.value;
      }
      return ok2({
        _tag: "If",
        span,
        condition: condition.value,
        property: null,
        consequent: consequentResult.value,
        alternative
      });
    }
    handleMatch(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const condition = this.handleOperands(syntax.scrutinee);
      if (!condition.ok) return condition;
      const branches = [];
      for (const branch of syntax.branches) {
        const cases = [];
        for (const caseItem of branch.cases) {
          const caseCondition = this.handleOperands(caseItem.condition);
          if (!caseCondition.ok) return caseCondition;
          cases.push({
            span: { start: caseItem.start, end: caseItem.end },
            condition: caseCondition.value
          });
        }
        const consequent = this.handleStatement(branch.consequent);
        if (!consequent.ok) return consequent;
        branches.push({
          span: { start: branch.start, end: branch.end },
          cases,
          consequent: consequent.value
        });
      }
      let alternative = null;
      if (syntax.alternative) {
        const altResult = this.handleCommand(syntax.alternative.alternative);
        if (!altResult.ok) return altResult;
        alternative = altResult.value;
      }
      return ok2({
        _tag: "Match",
        span,
        condition: condition.value,
        branches,
        alternative
      });
    }
    handleChoose(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const condition = this.handleOperands(syntax.condition);
      if (!condition.ok) return condition;
      const cases = [];
      if (syntax.cases) {
        for (const when of syntax.cases) {
          const whenResult = this.handleWhen(when);
          if (!whenResult.ok) return whenResult;
          cases.push(whenResult.value);
        }
      }
      let alternative = null;
      if (syntax.alternative) {
        const altResult = this.handleCommand(syntax.alternative.alternative);
        if (!altResult.ok) return altResult;
        alternative = altResult.value;
      }
      return ok2({
        _tag: "Choose",
        span,
        condition: condition.value,
        cases,
        alternative
      });
    }
    handleWhen(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const variant = [syntax.variant.name];
      if (syntax.variant.extensions) {
        for (const ext of syntax.variant.extensions) {
          variant.push(ext.name);
        }
      }
      const consequent = this.handleCommand(syntax.command);
      if (!consequent.ok) return consequent;
      return ok2({
        span,
        name: syntax.name,
        variant,
        consequent: consequent.value
      });
    }
    handleFor(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const expression = this.handleOperands(syntax.operation);
      if (!expression.ok) return expression;
      const actionResult = this.handleAction(syntax.action);
      if (!actionResult.ok) return actionResult;
      return ok2({
        _tag: "For",
        span,
        identifier: syntax.variable,
        expression: expression.value,
        action: actionResult.value
      });
    }
    handleWhile(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const conditionResult = this.handleCondition(syntax.condition);
      if (!conditionResult.ok) return conditionResult;
      const actionResult = this.handleAction(syntax.action);
      if (!actionResult.ok) return actionResult;
      return ok2({
        _tag: "While",
        span,
        condition: conditionResult.value,
        action: actionResult.value
      });
    }
    handleAction(syntax) {
      switch (syntax._syntax) {
        case "OperationSyntax": {
          const source = this.handleOperation(syntax);
          if (!source.ok) return source;
          return ok2({
            _tag: "Action",
            source: source.value,
            target: []
          });
        }
        case "SetSyntax": {
          const source = this.handleOperands(syntax.source);
          if (!source.ok) return source;
          const target = this.handleOperands(syntax.target);
          if (!target.ok) return target;
          return ok2({
            _tag: "Action",
            source: source.value,
            target: target.value
          });
        }
      }
    }
    handleCondition(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      switch (syntax._syntax) {
        case "OperationSyntax": {
          const operands = this.handleOperation(syntax);
          if (!operands.ok) return operands;
          return ok2({
            _tag: "Binding",
            span,
            bindingType: "",
            item: {
              span,
              private: false,
              name: null,
              type: null,
              attributes: []
            },
            operation: operands.value
          });
        }
        case "LetSyntax": {
          const itemResult = this.handleBindingItem(syntax.binding);
          if (!itemResult.ok) return itemResult;
          const operation = syntax.binding.operation ? this.handleOperands(syntax.binding.operation) : ok2([]);
          if (!operation.ok) return operation;
          return ok2({
            _tag: "Binding",
            span,
            bindingType: "let",
            item: itemResult.value,
            operation: operation.value
          });
        }
      }
    }
    handleTry(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const conditionResult = this.handleCondition(syntax.condition);
      if (!conditionResult.ok) return conditionResult;
      const catches = [];
      if (syntax.cases) {
        for (const when of syntax.cases) {
          const whenResult = this.handleWhen(when);
          if (!whenResult.ok) return whenResult;
          catches.push(whenResult.value);
        }
      }
      let alternative = null;
      if (syntax.dropper) {
        const altResult = this.handleCommand(syntax.dropper.alternative);
        if (!altResult.ok) return altResult;
        alternative = altResult.value;
      }
      return ok2({
        _tag: "Try",
        span,
        binding: conditionResult.value,
        catches,
        alternative
      });
    }
    // === Other Expressions ===
    handleSizeOf(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const type = this.handleType(syntax.type);
      if (!type.ok) return type;
      return ok2({
        _tag: "SizeOf",
        span,
        type: type.value
      });
    }
    handleIs(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const name = [syntax.name.name];
      if (syntax.name.extensions) {
        for (const ext of syntax.name.extensions) {
          name.push(ext.name);
        }
      }
      return ok2({
        _tag: "Is",
        span,
        name
      });
    }
    // === Item Handling ===
    handleBindingItem(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      let type = null;
      if (syntax.annotation) {
        const spec = syntax.annotation.spec;
        if (spec._syntax === "TypeSyntax") {
          const typeResult = this.handleType(spec);
          if (!typeResult.ok) return typeResult;
          type = typeResult.value;
        } else {
          return fail(notImplemented(this.file, spec._syntax, { start: spec.start, end: spec.end }));
        }
      }
      return ok2({
        span,
        private: false,
        name: syntax.name,
        type,
        attributes: []
      });
    }
    // === Attribute Handling ===
    handleAttribute(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const modelResult = this.handleModel(syntax.model);
      if (!modelResult.ok) return modelResult;
      return ok2({
        span,
        name: syntax.name,
        model: modelResult.value
      });
    }
    handleModel(syntax) {
      switch (syntax._syntax) {
        case "LiteralSyntax":
          return this.handleLiteral(syntax);
        case "NameSyntax":
          return ok2({
            _tag: "Variable",
            name: syntax.name
          });
        case "ObjectSyntax":
          return this.handleObject(syntax);
        case "VectorSyntax":
          return this.handleVector(syntax);
      }
    }
    // === Operand Handling ===
    handleOperand(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      let memberAccess = null;
      if (syntax.members) {
        memberAccess = [];
        for (const member of syntax.members) {
          memberAccess.push(member.name.name);
          if (member.name.extensions) {
            for (const ext of member.name.extensions) {
              memberAccess.push(ext.name);
            }
          }
        }
      }
      const expression = this.handleExpression(syntax.expression);
      if (!expression.ok) return expression;
      return ok2({
        span,
        expression: expression.value,
        memberAccess
      });
    }
    handleOperands(operands) {
      const result = [];
      for (const operand of operands) {
        const operandResult = this.handleOperand(operand);
        if (!operandResult.ok) return operandResult;
        result.push(operandResult.value);
      }
      return ok2(result);
    }
    handleOperation(syntax) {
      if (syntax.operands) {
        return this.handleOperands(syntax.operands);
      }
      return ok2([]);
    }
    // === Item Handling (for parameters) ===
    handleItem(syntax, isPrivate = false) {
      const span = { start: syntax.start, end: syntax.end };
      let type = null;
      if (syntax.annotation) {
        const typeResult = this.handleType(syntax.annotation.type);
        if (!typeResult.ok) return typeResult;
        type = typeResult.value;
      }
      const attributes = [];
      if (syntax.attributes) {
        for (const attrSyntax of syntax.attributes) {
          const attrResult = this.handleAttribute(attrSyntax);
          if (!attrResult.ok) return attrResult;
          attributes.push(attrResult.value);
        }
      }
      return ok2({
        span,
        private: isPrivate,
        name: syntax.name,
        type,
        attributes
      });
    }
    handleParameterSet(syntax) {
      const items = [];
      if (syntax._syntax === "ParametersSyntax") {
        if (syntax.items) {
          for (const itemSyntax of syntax.items) {
            const itemResult = this.handleItem(itemSyntax);
            if (!itemResult.ok) return itemResult;
            items.push(itemResult.value);
          }
        }
      } else if (syntax._syntax === "TypeSyntax") {
        const typeResult = this.handleType(syntax);
        if (!typeResult.ok) return typeResult;
        items.push({
          span: { start: syntax.start, end: syntax.end },
          private: false,
          name: null,
          type: typeResult.value,
          attributes: []
        });
      }
      return ok2(items);
    }
    // === Implementation Handling ===
    handleImplementation(syntax) {
      switch (syntax._syntax) {
        case "OperationSyntax": {
          const operands = this.handleOperation(syntax);
          if (!operands.ok) return operands;
          return ok2({
            _tag: "Action",
            source: operands.value,
            target: []
          });
        }
        case "SetSyntax": {
          const source = this.handleOperands(syntax.source);
          if (!source.ok) return source;
          const target = this.handleOperands(syntax.target);
          if (!target.ok) return target;
          return ok2({
            _tag: "Action",
            source: source.value,
            target: target.value
          });
        }
        case "ExternSyntax":
          return ok2({
            _tag: "Extern",
            span: { start: syntax.start, end: syntax.end }
          });
        case "InstructionSyntax":
          return ok2({
            _tag: "Instruction",
            span: { start: syntax.start, end: syntax.end }
          });
        case "IntrinsicSyntax":
          return ok2({
            _tag: "Intrinsic",
            span: { start: syntax.start, end: syntax.end }
          });
      }
    }
    // === Function Building ===
    buildFunction(syntax, isPrivate = false) {
      const span = { start: syntax.start, end: syntax.end };
      const target = syntax.target;
      if (target._syntax === "RoutineSyntax") {
        return fail(notImplemented(this.file, "Non-Symbol Function", span));
      }
      const named = target;
      const routine = named.routine;
      let input = [];
      if (routine.parameters) {
        const paramsResult = this.handleParameterSet(routine.parameters);
        if (!paramsResult.ok) return paramsResult;
        input = paramsResult.value;
      }
      let returns_ = null;
      if (routine.returns_) {
        const typeResult = this.handleType(routine.returns_.type);
        if (!typeResult.ok) return typeResult;
        returns_ = typeResult.value;
      }
      let throws_ = null;
      if (routine.throws_) {
        const typeResult = this.handleType(routine.throws_.type);
        if (!typeResult.ok) return typeResult;
        throws_ = typeResult.value;
      }
      let lifetime = { _tag: "Unspecified" };
      if (routine.lifetime) {
        const lt = routine.lifetime;
        switch (lt._syntax) {
          case "CallSyntax":
            lifetime = { _tag: "Call", span: { start: lt.start, end: lt.end } };
            break;
          case "LocalSyntax":
            lifetime = { _tag: "Local", span: { start: lt.start, end: lt.end } };
            break;
          case "ReferenceSyntax":
            lifetime = { _tag: "Reference", span: { start: lt.start, end: lt.end }, location: lt.location };
            break;
          case "ThrownSyntax":
            lifetime = { _tag: "Thrown", span: { start: lt.start, end: lt.end } };
            break;
        }
      }
      const implResult = this.handleImplementation(routine.implementation);
      if (!implResult.ok) return implResult;
      return ok2({
        _tag: "Function",
        span,
        private: isPrivate,
        pure: true,
        // function keyword means pure
        name: named.name,
        input,
        returns: returns_,
        throws: throws_,
        lifetime,
        implementation: implResult.value
      });
    }
    // === Procedure Building ===
    buildProcedure(syntax, isPrivate = false) {
      const span = { start: syntax.start, end: syntax.end };
      const target = syntax.target;
      if (target._syntax === "RoutineSyntax") {
        return fail(notImplemented(this.file, "Non-Symbol Procedure", span));
      }
      const named = target;
      const routine = named.routine;
      let input = [];
      if (routine.parameters) {
        const paramsResult = this.handleParameterSet(routine.parameters);
        if (!paramsResult.ok) return paramsResult;
        input = paramsResult.value;
      }
      let returns_ = null;
      if (routine.returns_) {
        const typeResult = this.handleType(routine.returns_.type);
        if (!typeResult.ok) return typeResult;
        returns_ = typeResult.value;
      }
      let throws_ = null;
      if (routine.throws_) {
        const typeResult = this.handleType(routine.throws_.type);
        if (!typeResult.ok) return typeResult;
        throws_ = typeResult.value;
      }
      let lifetime = { _tag: "Unspecified" };
      if (routine.lifetime) {
        const lt = routine.lifetime;
        switch (lt._syntax) {
          case "CallSyntax":
            lifetime = { _tag: "Call", span: { start: lt.start, end: lt.end } };
            break;
          case "LocalSyntax":
            lifetime = { _tag: "Local", span: { start: lt.start, end: lt.end } };
            break;
          case "ReferenceSyntax":
            lifetime = { _tag: "Reference", span: { start: lt.start, end: lt.end }, location: lt.location };
            break;
          case "ThrownSyntax":
            lifetime = { _tag: "Thrown", span: { start: lt.start, end: lt.end } };
            break;
        }
      }
      const implResult = this.handleImplementation(routine.implementation);
      if (!implResult.ok) return implResult;
      return ok2({
        _tag: "Function",
        span,
        private: isPrivate,
        pure: false,
        // procedure keyword means impure
        name: named.name,
        input,
        returns: returns_,
        throws: throws_,
        lifetime,
        implementation: implResult.value
      });
    }
    // === Operator Building ===
    buildOperator(syntax, isPrivate = false) {
      const span = { start: syntax.start, end: syntax.end };
      const target = syntax.target;
      let input = [];
      let name;
      if (target._syntax === "RoutineSyntax") {
        const routine2 = target;
        if (routine2.parameters) {
          const paramsResult = this.handleParameterSet(routine2.parameters);
          if (!paramsResult.ok) return paramsResult;
          input = paramsResult.value;
        }
        name = "[]";
        let returns_2 = null;
        if (routine2.returns_) {
          const typeResult = this.handleType(routine2.returns_.type);
          if (!typeResult.ok) return typeResult;
          returns_2 = typeResult.value;
        }
        let throws_2 = null;
        if (routine2.throws_) {
          const typeResult = this.handleType(routine2.throws_.type);
          if (!typeResult.ok) return typeResult;
          throws_2 = typeResult.value;
        }
        const implResult2 = this.handleImplementation(routine2.implementation);
        if (!implResult2.ok) return implResult2;
        return ok2({
          _tag: "Operator",
          span,
          private: isPrivate,
          name,
          input,
          returns: returns_2,
          throws: throws_2,
          implementation: implResult2.value
        });
      }
      const named = target;
      const routine = named.routine;
      name = named.name;
      if (routine.parameters) {
        const paramsResult = this.handleParameterSet(routine.parameters);
        if (!paramsResult.ok) return paramsResult;
        input = paramsResult.value;
      }
      let returns_ = null;
      if (routine.returns_) {
        const typeResult = this.handleType(routine.returns_.type);
        if (!typeResult.ok) return typeResult;
        returns_ = typeResult.value;
      }
      let throws_ = null;
      if (routine.throws_) {
        const typeResult = this.handleType(routine.throws_.type);
        if (!typeResult.ok) return typeResult;
        throws_ = typeResult.value;
      }
      const implResult = this.handleImplementation(routine.implementation);
      if (!implResult.ok) return implResult;
      return ok2({
        _tag: "Operator",
        span,
        private: isPrivate,
        name,
        input,
        returns: returns_,
        throws: throws_,
        implementation: implResult.value
      });
    }
    // === Definition Handling ===
    handleDefinition(syntax, _isPrivate = false) {
      const span = { start: syntax.start, end: syntax.end };
      const parameters = [];
      if (syntax.parameters) {
        for (const param of syntax.parameters.parameters) {
          const attributes2 = [];
          if (param.attributes) {
            for (const attr of param.attributes) {
              const attrResult = this.handleAttribute(attr);
              if (!attrResult.ok) return attrResult;
              attributes2.push(attrResult.value);
            }
          }
          parameters.push({
            span: { start: param.start, end: param.end },
            name: param.name,
            attributes: attributes2
          });
        }
      }
      const attributes = [];
      if (syntax.attributes) {
        for (const attr of syntax.attributes) {
          const attrResult = this.handleAttribute(attr);
          if (!attrResult.ok) return attrResult;
          attributes.push(attrResult.value);
        }
      }
      const definitionResult = this.handleConcept(syntax.concept_);
      if (!definitionResult.ok) return definitionResult;
      return ok2({
        _tag: "Concept",
        span,
        name: syntax.name,
        parameters,
        attributes,
        definition: definitionResult.value
      });
    }
    handleConcept(syntax) {
      switch (syntax._syntax) {
        case "IntrinsicSyntax":
          return ok2({
            _tag: "Intrinsic",
            span: { start: syntax.start, end: syntax.end }
          });
        case "ClassSyntax":
          return this.handleClass(syntax);
        case "NamespaceSyntax":
          return this.handleNamespace(syntax);
        case "UnionSyntax":
          return this.handleUnion(syntax);
        case "ConstantSyntax":
          return fail(notImplemented(this.file, "ConstantSyntax", { start: syntax.start, end: syntax.end }));
        case "DelegateSyntax":
          return fail(notImplemented(this.file, "DelegateSyntax", { start: syntax.start, end: syntax.end }));
      }
    }
    handleClass(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const properties = [];
      if (syntax.structure.parts) {
        for (const part of syntax.structure.parts) {
          const prop = part._syntax === "FieldSyntax" ? part.property : part;
          const propResult = this.handleProperty(prop, false);
          if (!propResult.ok) return propResult;
          properties.push(propResult.value);
        }
      }
      const members = [];
      const symbols = /* @__PURE__ */ new Map();
      if (syntax.body) {
        const bodyResult = this.handleBody(syntax.body, members, symbols);
        if (!bodyResult.ok) return bodyResult;
      }
      return ok2({
        _tag: "Structure",
        span,
        private: false,
        properties,
        modules: [],
        uses: [],
        initializers: [],
        deinitializer: null,
        members,
        symbols
      });
    }
    handleProperty(syntax, isPrivate) {
      const span = { start: syntax.start, end: syntax.end };
      let type = null;
      if (syntax.annotation) {
        const typeResult = this.handleType(syntax.annotation.type);
        if (!typeResult.ok) return typeResult;
        type = typeResult.value;
      }
      let initializer = null;
      if (syntax.initializer) {
        const initResult = this.handleOperands(syntax.initializer.operands);
        if (!initResult.ok) return initResult;
        initializer = initResult.value;
      }
      const attributes = [];
      if (syntax.attributes) {
        for (const attr of syntax.attributes) {
          const attrResult = this.handleAttribute(attr);
          if (!attrResult.ok) return attrResult;
          attributes.push(attrResult.value);
        }
      }
      return ok2({
        span,
        private: isPrivate,
        name: syntax.name,
        type,
        initializer,
        attributes
      });
    }
    handleNamespace(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const members = [];
      const symbols = /* @__PURE__ */ new Map();
      if (syntax.declarations) {
        for (const decl of syntax.declarations) {
          const memberResult = this.handleDeclaration(decl);
          if (!memberResult.ok) return memberResult;
          members.push(memberResult.value);
          const member = memberResult.value;
          if (member._tag !== "Package" && "name" in member) {
            symbols.set(member.name, member);
          }
        }
      }
      return ok2({
        _tag: "Namespace",
        span,
        modules: [],
        members,
        symbols
      });
    }
    handleUnion(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      const variants = [];
      if (syntax.variants) {
        for (const v of syntax.variants) {
          const variantResult = this.handleVariant(v);
          if (!variantResult.ok) return variantResult;
          variants.push(variantResult.value);
        }
      }
      const members = [];
      const symbols = /* @__PURE__ */ new Map();
      if (syntax.body) {
        const bodyResult = this.handleBody(syntax.body, members, symbols);
        if (!bodyResult.ok) return bodyResult;
      }
      return ok2({
        _tag: "Union",
        span,
        private: false,
        variants,
        members,
        symbols
      });
    }
    handleVariant(syntax) {
      const span = { start: syntax.start, end: syntax.end };
      let type = null;
      if (syntax.annotation) {
        const typeResult = this.handleType(syntax.annotation.type);
        if (!typeResult.ok) return typeResult;
        type = typeResult.value;
      }
      const attributes = [];
      if (syntax.attributes) {
        for (const attr of syntax.attributes) {
          const attrResult = this.handleAttribute(attr);
          if (!attrResult.ok) return attrResult;
          attributes.push(attrResult.value);
        }
      }
      return ok2({
        span,
        name: syntax.name,
        type,
        attributes
      });
    }
    handleDeclaration(decl) {
      const symbol = decl.symbol;
      const isPrivate = symbol._syntax === "PrivateSyntax";
      const actual = isPrivate ? symbol.export_ : symbol;
      switch (actual._syntax) {
        case "DefinitionSyntax":
          return this.handleDefinition(actual, isPrivate);
        case "FunctionSyntax":
          return this.buildFunction(actual, isPrivate);
        case "OperatorSyntax":
          return this.buildOperator(actual, isPrivate);
        case "ModuleSyntax":
        case "TraitSyntax":
          return fail(notImplemented(this.file, actual._syntax, { start: actual.start, end: actual.end }));
      }
    }
    handleBody(syntax, members, symbols) {
      if (syntax.members) {
        for (const member of syntax.members) {
          const constituent = member.constituent;
          switch (constituent._syntax) {
            case "DefinitionSyntax": {
              const conceptResult = this.handleDefinition(constituent, false);
              if (!conceptResult.ok) return conceptResult;
              members.push(conceptResult.value);
              symbols.set(conceptResult.value.name, conceptResult.value);
              break;
            }
            case "FunctionSyntax": {
              const funcResult = this.buildFunction(constituent, false);
              if (!funcResult.ok) return funcResult;
              members.push(funcResult.value);
              const existing = symbols.get(funcResult.value.name);
              if (existing && existing._tag === "Functions") {
                existing.functions.push(funcResult.value);
              } else {
                symbols.set(funcResult.value.name, { _tag: "Functions", functions: [funcResult.value] });
              }
              break;
            }
            case "ProcedureSyntax": {
              const procResult = this.buildProcedure(constituent, false);
              if (!procResult.ok) return procResult;
              members.push(procResult.value);
              const existing = symbols.get(procResult.value.name);
              if (existing && existing._tag === "Functions") {
                existing.functions.push(procResult.value);
              } else {
                symbols.set(procResult.value.name, { _tag: "Functions", functions: [procResult.value] });
              }
              break;
            }
            case "OperatorSyntax": {
              const opResult = this.buildOperator(constituent, false);
              if (!opResult.ok) return opResult;
              members.push(opResult.value);
              symbols.set(opResult.value.name, opResult.value);
              break;
            }
            case "ModuleSyntax": {
              break;
            }
            default:
              return fail(notImplemented(this.file, constituent._syntax, { start: constituent.start, end: constituent.end }));
          }
        }
      }
      return ok2(void 0);
    }
    // === File Handling ===
    handleFile(syntax) {
      const members = [];
      const symbols = /* @__PURE__ */ new Map();
      const uses = [];
      if (syntax.uses) {
        for (const useSyntax of syntax.uses) {
          const path = [useSyntax.name.name];
          if (useSyntax.name.extensions) {
            for (const ext of useSyntax.name.extensions) {
              path.push(ext.name);
            }
          }
          uses.push({ span: { start: useSyntax.start, end: useSyntax.end }, path });
        }
      }
      if (syntax.declarations) {
        for (const decl of syntax.declarations) {
          const symbol = decl.symbol;
          const isPrivate = symbol._syntax === "PrivateSyntax";
          const actual = isPrivate ? symbol.export_ : symbol;
          switch (actual._syntax) {
            case "DefinitionSyntax": {
              const conceptResult = this.handleDefinition(actual, isPrivate);
              if (!conceptResult.ok) return conceptResult;
              members.push(conceptResult.value);
              symbols.set(conceptResult.value.name, conceptResult.value);
              break;
            }
            case "FunctionSyntax": {
              const funcResult = this.buildFunction(actual, isPrivate);
              if (!funcResult.ok) return funcResult;
              members.push(funcResult.value);
              const existing = symbols.get(funcResult.value.name);
              if (existing && existing._tag === "Functions") {
                existing.functions.push(funcResult.value);
              } else {
                symbols.set(funcResult.value.name, { _tag: "Functions", functions: [funcResult.value] });
              }
              break;
            }
            case "OperatorSyntax": {
              const opResult = this.buildOperator(actual, isPrivate);
              if (!opResult.ok) return opResult;
              members.push(opResult.value);
              symbols.set(opResult.value.name, opResult.value);
              break;
            }
            case "ModuleSyntax":
            case "TraitSyntax":
              break;
            default:
              return fail(notImplemented(this.file, actual._syntax, { start: actual.start, end: actual.end }));
          }
        }
      }
      return ok2({
        private: false,
        file: this.file,
        name: "",
        modules: [],
        uses,
        members,
        symbols
      });
    }
  };
  function parseAndModel(input, file = "<input>") {
    const parser = new Parser(input);
    const parseResult = parser.parseOperation();
    if (!parseResult.ok) {
      return fail(parserError(file, parseResult.error));
    }
    const modeler = new Modeler(file);
    return modeler.modelOperation(parseResult.value);
  }
  function parseAndModelFile(input, file = "<input>") {
    const parser = new Parser(input);
    const parseResult = parser.parseFile();
    if (!parseResult.ok) {
      return fail(parserError(file, parseResult.error));
    }
    const modeler = new Modeler(file);
    return modeler.handleFile(parseResult.value);
  }

  // src/runtime.browser.ts
  var STDLIB_CONTENT = `; Scaly Standard Library
; Arithmetic operators with precedence via wrapper types

; -----------------------------------------------------------------------------
; Precedence Wrapper Types
; -----------------------------------------------------------------------------

define Sum(left: int, right: int) {
    function value(this) returns int intrinsic
}

define Difference(left: int, right: int) {
    function value(this) returns int intrinsic
}

define Product(left: int, right: int) {
    function value(this) returns int intrinsic
}

define Quotient(left: int, right: int) {
    function value(this) returns int intrinsic
}

; -----------------------------------------------------------------------------
; Addition Operators
; -----------------------------------------------------------------------------

; Primitive: creates Sum wrapper
operator +(left: int, right: int) returns Sum intrinsic

; These evaluate the left wrapper first
operator +(left: Sum, right: int) returns Sum {
    Sum(left.value, right)
}

operator +(left: Difference, right: int) returns Sum {
    Sum(left.value, right)
}

operator +(left: Product, right: int) returns Sum {
    Sum(left.value, right)
}

operator +(left: Quotient, right: int) returns Sum {
    Sum(left.value, right)
}

; -----------------------------------------------------------------------------
; Subtraction Operators
; -----------------------------------------------------------------------------

; Primitive: creates Difference wrapper
operator -(left: int, right: int) returns Difference intrinsic

operator -(left: Sum, right: int) returns Difference {
    Difference(left.value, right)
}

operator -(left: Difference, right: int) returns Difference {
    Difference(left.value, right)
}

operator -(left: Product, right: int) returns Difference {
    Difference(left.value, right)
}

operator -(left: Quotient, right: int) returns Difference {
    Difference(left.value, right)
}

; -----------------------------------------------------------------------------
; Multiplication Operators
; -----------------------------------------------------------------------------

; Primitive: creates Product wrapper
operator *(left: int, right: int) returns Product intrinsic

; Higher precedence: reaches into Sum/Difference
operator *(left: Sum, right: int) returns Sum {
    Sum(left.left, left.right * right)
}

operator *(left: Difference, right: int) returns Difference {
    Difference(left.left, left.right * right)
}

operator *(left: Product, right: int) returns Product {
    Product(left.value, right)
}

operator *(left: Quotient, right: int) returns Product {
    Product(left.value, right)
}

; -----------------------------------------------------------------------------
; Division Operators
; -----------------------------------------------------------------------------

; Primitive: creates Quotient wrapper
operator /(left: int, right: int) returns Quotient intrinsic

; Higher precedence: reaches into Sum/Difference
operator /(left: Sum, right: int) returns Sum {
    Sum(left.left, left.right / right)
}

operator /(left: Difference, right: int) returns Difference {
    Difference(left.left, left.right / right)
}

operator /(left: Product, right: int) returns Quotient {
    Quotient(left.value, right)
}

operator /(left: Quotient, right: int) returns Quotient {
    Quotient(left.value, right)
}

; -----------------------------------------------------------------------------
; Integer Division and Modulo
; -----------------------------------------------------------------------------

operator div(left: int, right: int) returns int intrinsic
operator mod(left: int, right: int) returns int intrinsic

; -----------------------------------------------------------------------------
; Comparison Operators
; -----------------------------------------------------------------------------

operator =(left: int, right: int) returns bool intrinsic
operator <>(left: int, right: int) returns bool intrinsic
operator <(left: int, right: int) returns bool intrinsic
operator >(left: int, right: int) returns bool intrinsic
operator <=(left: int, right: int) returns bool intrinsic
operator >=(left: int, right: int) returns bool intrinsic

; -----------------------------------------------------------------------------
; Unary Operators
; -----------------------------------------------------------------------------

function -(right: int) returns int intrinsic
function +(right: int) returns int intrinsic

; -----------------------------------------------------------------------------
; Math Functions
; -----------------------------------------------------------------------------

function abs(x: int) returns int intrinsic
`;
  var Runtime = class {
    operators = /* @__PURE__ */ new Map();
    functions = /* @__PURE__ */ new Map();
    types = /* @__PURE__ */ new Map();
    // Load stdlib from embedded content
    loadStdlib() {
      const result = parseAndModelFile(STDLIB_CONTENT, "stdlib.scaly");
      if (!result.ok) {
        return { ok: false, error: `Failed to parse stdlib: ${formatModelError(result.error, STDLIB_CONTENT)}` };
      }
      for (const member of result.value.members) {
        if (member._tag === "Concept") {
          this.types.set(member.name, member);
        } else if (member._tag === "Operator") {
          this.registerOperator(member);
        } else if (member._tag === "Function") {
          this.registerFunction(member);
        }
      }
      return { ok: true };
    }
    registerOperator(op) {
      const name = op.name;
      const signatures = this.operators.get(name) || [];
      const leftParam = op.input[0];
      const rightParam = op.input[1];
      const leftType = leftParam?.type ? this.typeToString(leftParam.type) : "int";
      const rightType = rightParam?.type ? this.typeToString(rightParam.type) : "int";
      const returnType = op.returns ? this.typeToString(op.returns) : "void";
      const intrinsic = op.implementation?._tag === "Intrinsic";
      signatures.push({
        leftType,
        rightType,
        returnType,
        intrinsic,
        implementation: op.implementation
      });
      this.operators.set(name, signatures);
    }
    registerFunction(fn) {
      const name = fn.name;
      const signatures = this.functions.get(name) || [];
      const param = fn.input[0];
      const paramType = param?.type ? this.typeToString(param.type) : "int";
      const returnType = fn.returns ? this.typeToString(fn.returns) : "void";
      const intrinsic = fn.implementation?._tag === "Intrinsic";
      signatures.push({
        paramType,
        returnType,
        intrinsic,
        implementation: fn.implementation
      });
      this.functions.set(name, signatures);
    }
    typeToString(type) {
      return type.name.join(".");
    }
    // === Evaluation ===
    evaluate(operands, scope = /* @__PURE__ */ new Map()) {
      if (operands.length === 0) {
        return { ok: true, value: { _tag: "Unit" } };
      }
      const firstExpr = operands[0].expression;
      if (firstExpr._tag === "Type" && operands.length >= 2) {
        const name = firstExpr.name[0];
        if (this.types.has(name)) {
          const secondExpr = operands[1].expression;
          if (secondExpr._tag === "Tuple") {
            const result = this.constructWrapper(name, secondExpr, scope);
            if (!result.ok) return result;
            if (operands.length > 2) {
              return this.evaluateWithContext(result.value, operands.slice(2), scope);
            }
            return result;
          }
        }
        if (this.functions.has(name)) {
          const secondExpr = operands[1].expression;
          if (secondExpr._tag === "Tuple") {
            if (secondExpr.components.length === 1 && !secondExpr.components[0].name) {
              const argResult = this.evaluate(secondExpr.components[0].value, scope);
              if (!argResult.ok) return argResult;
              const collapsed = this.collapseWrapper(argResult.value);
              if (!collapsed.ok) return collapsed;
              const funcResult = this.applyFunction(name, collapsed.value, scope);
              if (!funcResult.ok) return funcResult;
              if (operands.length > 2) {
                return this.evaluateWithContext(funcResult.value, operands.slice(2), scope);
              }
              return funcResult;
            }
          }
        }
        if (this.functions.has(name)) {
          const argResult = this.evaluateOperand(operands[1], scope);
          if (!argResult.ok) return argResult;
          const collapsed = this.collapseWrapper(argResult.value);
          if (!collapsed.ok) return collapsed;
          const funcResult = this.applyFunction(name, collapsed.value, scope);
          if (!funcResult.ok) return funcResult;
          if (operands.length > 2) {
            return this.evaluateWithContext(funcResult.value, operands.slice(2), scope);
          }
          return funcResult;
        }
      }
      let context = this.evaluateOperand(operands[0], scope);
      if (!context.ok) return context;
      for (let i = 1; i < operands.length; i++) {
        const operand = operands[i];
        const expr = operand.expression;
        if (expr._tag === "Type") {
          const name = expr.name[0];
          if (this.types.has(name) && i + 1 < operands.length) {
            const nextExpr = operands[i + 1].expression;
            if (nextExpr._tag === "Tuple") {
              const result = this.constructWrapper(name, nextExpr, scope);
              if (!result.ok) return result;
              context = result;
              i++;
              continue;
            }
          }
          if (i + 1 < operands.length) {
            const rightOperand = operands[i + 1];
            const rightExpr = rightOperand.expression;
            if (rightExpr._tag === "Type" && this.operators.has(rightExpr.name[0])) {
              const result = this.applyFunction(name, context.value, scope);
              if (!result.ok) return result;
              context = result;
            } else {
              const rightValue = this.evaluateOperand(rightOperand, scope);
              if (!rightValue.ok) return rightValue;
              const result = this.applyOperator(name, context.value, rightValue.value, scope);
              if (!result.ok) return result;
              context = result;
              i++;
            }
          } else {
            const contextType = this.valueType(context.value);
            if (this.hasBinarySignature(name, contextType)) {
              return { ok: false, error: `Operator '${name}' requires a right operand` };
            }
            const result = this.applyFunction(name, context.value, scope);
            if (!result.ok) return result;
            context = result;
          }
        } else {
          return { ok: false, error: `Unexpected operand: ${expr._tag}` };
        }
      }
      return context;
    }
    evaluateWithContext(context, operands, scope) {
      let current = { ok: true, value: context };
      for (let i = 0; i < operands.length; i++) {
        const operand = operands[i];
        const expr = operand.expression;
        if (expr._tag === "Type") {
          const name = expr.name[0];
          if (i + 1 < operands.length) {
            const rightOperand = operands[i + 1];
            const rightExpr = rightOperand.expression;
            if (rightExpr._tag === "Type" && this.operators.has(rightExpr.name[0])) {
              const result = this.applyFunction(name, current.value, scope);
              if (!result.ok) return result;
              current = result;
            } else {
              const rightValue = this.evaluateOperand(rightOperand, scope);
              if (!rightValue.ok) return rightValue;
              const result = this.applyOperator(name, current.value, rightValue.value, scope);
              if (!result.ok) return result;
              current = result;
              i++;
            }
          } else {
            const contextType = this.valueType(current.value);
            if (this.hasBinarySignature(name, contextType)) {
              return { ok: false, error: `Operator '${name}' requires a right operand` };
            }
            const result = this.applyFunction(name, current.value, scope);
            if (!result.ok) return result;
            current = result;
          }
        }
      }
      return current;
    }
    constructWrapper(typeName, tuple, scope) {
      const fields = /* @__PURE__ */ new Map();
      const fieldNames = ["left", "right"];
      for (let i = 0; i < tuple.components.length && i < fieldNames.length; i++) {
        const component = tuple.components[i];
        const fieldResult = this.evaluate(component.value, scope);
        if (!fieldResult.ok) return fieldResult;
        const collapsed = this.collapseWrapper(fieldResult.value);
        if (!collapsed.ok) return collapsed;
        fields.set(fieldNames[i], collapsed.value);
      }
      return { ok: true, value: { _tag: "Wrapper", type: typeName, fields } };
    }
    evaluateOperand(operand, scope) {
      let value = this.evaluateExpression(operand.expression, scope);
      if (!value.ok) return value;
      if (operand.memberAccess && operand.memberAccess.length > 0) {
        for (const member of operand.memberAccess) {
          if (value.value._tag !== "Wrapper") {
            return { ok: false, error: `Cannot access member '${member}' on ${value.value._tag}` };
          }
          const field = value.value.fields.get(member);
          if (!field) {
            return { ok: false, error: `Unknown field '${member}' on ${value.value.type}` };
          }
          value = { ok: true, value: field };
        }
      }
      return value;
    }
    evaluateExpression(expr, scope) {
      switch (expr._tag) {
        case "IntegerConstant":
        case "HexConstant":
          return { ok: true, value: { _tag: "Int", value: expr.value } };
        case "FloatingPointConstant":
          return { ok: true, value: { _tag: "Float", value: expr.value } };
        case "BooleanConstant":
          return { ok: true, value: { _tag: "Bool", value: expr.value } };
        case "StringConstant":
          return { ok: true, value: { _tag: "String", value: expr.value } };
        case "CharacterConstant":
          return { ok: true, value: { _tag: "Char", value: expr.value } };
        case "FragmentConstant":
          return { ok: true, value: { _tag: "Char", value: expr.value } };
        case "Tuple":
          if (expr.components.length === 1 && !expr.components[0].name) {
            const innerResult = this.evaluate(expr.components[0].value, scope);
            if (!innerResult.ok) return innerResult;
            return this.collapseWrapper(innerResult.value);
          }
          return { ok: false, error: "Complex tuples not yet implemented" };
        case "Block":
          return this.evaluateBlock(expr, scope);
        case "Type":
          const name = expr.name[0];
          if (name === "true" && expr.name.length === 1) {
            return { ok: true, value: { _tag: "Bool", value: true } };
          }
          if (name === "false" && expr.name.length === 1) {
            return { ok: true, value: { _tag: "Bool", value: false } };
          }
          const scopeValue = scope.get(name);
          if (scopeValue) {
            if (expr.name.length > 1) {
              let value = scopeValue;
              for (let i = 1; i < expr.name.length; i++) {
                const memberName = expr.name[i];
                if (value._tag !== "Wrapper") {
                  return { ok: false, error: `Cannot access member '${memberName}' on ${value._tag}` };
                }
                if (memberName === "value") {
                  const collapsed = this.collapseWrapper(value);
                  if (!collapsed.ok) return collapsed;
                  value = collapsed.value;
                  continue;
                }
                const field = value.fields.get(memberName);
                if (!field) {
                  return { ok: false, error: `Unknown field '${memberName}' on ${value.type}` };
                }
                value = field;
              }
              return { ok: true, value };
            }
            return { ok: true, value: scopeValue };
          }
          if (this.types.has(name)) {
            return { ok: false, error: `Constructor ${name} needs arguments` };
          }
          return { ok: false, error: `Unresolved reference: ${expr.name.join(".")}` };
        default:
          return { ok: false, error: `Expression type not implemented: ${expr._tag}` };
      }
    }
    evaluateBlock(block, scope) {
      let lastValue = { _tag: "Unit" };
      for (const stmt of block.statements) {
        if (stmt._tag === "Action") {
          const result = this.evaluate(stmt.source, scope);
          if (!result.ok) return result;
          lastValue = result.value;
        } else if (stmt._tag === "Return") {
          const result = this.evaluate(stmt.result, scope);
          if (!result.ok) return result;
          return result;
        }
      }
      return { ok: true, value: lastValue };
    }
    applyOperator(name, left, right, scope) {
      const signatures = this.operators.get(name);
      if (!signatures) {
        return { ok: false, error: `Unknown operator: ${name}` };
      }
      const leftType = this.valueType(left);
      const rightType = this.valueType(right);
      const sig = signatures.find((s) => s.leftType === leftType && s.rightType === rightType);
      if (!sig) {
        return { ok: false, error: `No matching operator ${name}(${leftType}, ${rightType})` };
      }
      if (sig.intrinsic) {
        return this.applyIntrinsicOperator(name, left, right, sig.returnType);
      }
      if (sig.implementation && sig.implementation._tag === "Action") {
        const innerScope = new Map(scope);
        innerScope.set("left", left);
        innerScope.set("right", right);
        return this.evaluate(sig.implementation.source, innerScope);
      }
      return { ok: false, error: `No implementation for operator ${name}(${leftType}, ${rightType})` };
    }
    applyFunction(name, arg, scope) {
      const signatures = this.functions.get(name);
      if (!signatures) {
        return { ok: false, error: `Unknown function: ${name}` };
      }
      const argType = this.valueType(arg);
      const sig = signatures.find((s) => s.paramType === argType);
      if (!sig) {
        return { ok: false, error: `No matching function ${name}(${argType})` };
      }
      if (sig.intrinsic) {
        return this.applyIntrinsicFunction(name, arg, sig.returnType);
      }
      if (sig.implementation && sig.implementation._tag === "Action") {
        const innerScope = new Map(scope);
        innerScope.set("arg", arg);
        return this.evaluate(sig.implementation.source, innerScope);
      }
      return { ok: false, error: `Non-intrinsic functions not yet implemented` };
    }
    valueType(value) {
      switch (value._tag) {
        case "Int":
          return "int";
        case "Bool":
          return "bool";
        case "Float":
          return "float";
        case "String":
          return "string";
        case "Char":
          return "char";
        case "Wrapper":
          return value.type;
        case "Unit":
          return "void";
      }
    }
    // Check if operator has a binary signature for the given left type
    hasBinarySignature(name, leftType) {
      const signatures = this.operators.get(name);
      if (!signatures) return false;
      return signatures.some((s) => s.leftType === leftType);
    }
    // === Intrinsic Implementations ===
    applyIntrinsicOperator(name, left, right, _returnType) {
      if (left._tag !== "Int" || right._tag !== "Int") {
        return { ok: false, error: `Intrinsic ${name} only implemented for int` };
      }
      const l = left.value;
      const r = right.value;
      switch (name) {
        case "+":
          return { ok: true, value: this.makeWrapper("Sum", l, r) };
        case "-":
          return { ok: true, value: this.makeWrapper("Difference", l, r) };
        case "*":
          return { ok: true, value: this.makeWrapper("Product", l, r) };
        case "/":
          return { ok: true, value: this.makeWrapper("Quotient", l, r) };
        case "div":
          return { ok: true, value: { _tag: "Int", value: Math.trunc(l / r) } };
        case "mod":
          return { ok: true, value: { _tag: "Int", value: l % r } };
        case "=":
          return { ok: true, value: { _tag: "Bool", value: l === r } };
        case "<>":
          return { ok: true, value: { _tag: "Bool", value: l !== r } };
        case "<":
          return { ok: true, value: { _tag: "Bool", value: l < r } };
        case ">":
          return { ok: true, value: { _tag: "Bool", value: l > r } };
        case "<=":
          return { ok: true, value: { _tag: "Bool", value: l <= r } };
        case ">=":
          return { ok: true, value: { _tag: "Bool", value: l >= r } };
        default:
          return { ok: false, error: `Unknown intrinsic operator: ${name}` };
      }
    }
    applyIntrinsicFunction(name, arg, _returnType) {
      if (arg._tag !== "Int") {
        return { ok: false, error: `Intrinsic ${name} only implemented for int` };
      }
      const v = arg.value;
      switch (name) {
        case "-":
          return { ok: true, value: { _tag: "Int", value: -v } };
        case "+":
          return { ok: true, value: { _tag: "Int", value: v } };
        case "abs":
          return { ok: true, value: { _tag: "Int", value: Math.abs(v) } };
        default:
          return { ok: false, error: `Unknown intrinsic function: ${name}` };
      }
    }
    makeWrapper(type, left, right) {
      const fields = /* @__PURE__ */ new Map();
      fields.set("left", { _tag: "Int", value: left });
      fields.set("right", { _tag: "Int", value: right });
      return { _tag: "Wrapper", type, fields };
    }
    // Collapse a wrapper to its value (for expression boundaries like parentheses)
    collapseWrapper(value) {
      if (value._tag !== "Wrapper") {
        return { ok: true, value };
      }
      const left = value.fields.get("left");
      const right = value.fields.get("right");
      if (!left || !right || left._tag !== "Int" || right._tag !== "Int") {
        return { ok: false, error: `Invalid wrapper structure` };
      }
      switch (value.type) {
        case "Sum":
          return { ok: true, value: { _tag: "Int", value: left.value + right.value } };
        case "Difference":
          return { ok: true, value: { _tag: "Int", value: left.value - right.value } };
        case "Product":
          return { ok: true, value: { _tag: "Int", value: left.value * right.value } };
        case "Quotient":
          return { ok: true, value: { _tag: "Int", value: left.value / right.value } };
        default:
          return { ok: false, error: `Unknown wrapper type: ${value.type}` };
      }
    }
    // Format a value for display
    formatValue(value) {
      switch (value._tag) {
        case "Int":
          return String(value.value);
        case "Bool":
          return String(value.value);
        case "Float":
          return String(value.value);
        case "String":
          return JSON.stringify(value.value);
        case "Char":
          return `'${value.value}'`;
        case "Wrapper":
          return `${value.type}(${this.formatValue(value.fields.get("left"))}, ${this.formatValue(value.fields.get("right"))})`;
        case "Unit":
          return "()";
      }
    }
  };

  // src/browser.ts
  var runtime = null;
  function getRuntime() {
    if (!runtime) {
      runtime = new Runtime();
      const result = runtime.loadStdlib();
      if (!result.ok) {
        throw new Error(result.error);
      }
    }
    return runtime;
  }
  function evaluate(input) {
    const modelResult = parseAndModel(input);
    if (!modelResult.ok) {
      return { _tag: "ModelError", message: formatModelError(modelResult.error, input) };
    }
    const rt = getRuntime();
    const evalResult = rt.evaluate(modelResult.value);
    if (!evalResult.ok) {
      return { _tag: "EvalError", message: evalResult.error, code: input };
    }
    const collapsed = rt.collapseWrapper(evalResult.value);
    if (!collapsed.ok) {
      return { _tag: "EvalError", message: collapsed.error, code: input };
    }
    const value = valueToJs(collapsed.value);
    return { _tag: "Ok", value, code: input };
  }
  function valueToJs(value) {
    switch (value._tag) {
      case "Int":
      case "Float":
        return value.value;
      case "Bool":
        return value.value;
      case "String":
      case "Char":
        return value.value;
      case "Wrapper":
        return `${value.type}(...)`;
      case "Unit":
        return void 0;
    }
  }
  function resetEvaluator() {
    runtime = null;
  }
  return __toCommonJS(browser_exports);
})();
