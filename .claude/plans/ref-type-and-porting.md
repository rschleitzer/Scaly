# Plan: ref[T] Typ und Compiler-Portierung nach Scaly

## Kontext

Scaly soll `pointer[T]` aus der Anwendungsprogrammierung raushalten. Container wie Vector/HashMap geben aktuell pointer zurueck (Relikt aus Transpiler-Aera). Loesung: `ref[T]` als RBMM-native Referenz.

## ref[T] Design

### Konzept
- `ref[T]` ist eine nicht-besitzende Referenz auf Daten in einer Region
- Implizit an die Lebenszeit der Region gebunden
- Kann die Region nicht ueberleben (Compiler-Check)
- Kein voller Borrow-Checker noetig - Region-Scope ist der Lifetime

### Regeln
1. `ref[T]` kann nie die Region ueberleben, aus der es stammt
2. `ref[T]` kann nicht in laenger lebende Region "escapen"
3. Dereferenzierung mit `*ref` liefert Kopie des Werts
4. `pointer[T]` bleibt nur fuer interne Container-Implementierung

### Container-API Pattern
```scaly
define Vector[T](length: size_t, data: pointer[T])
{
    ; Referenz - lebt so lange wie Vector
    function get(this, i: size_t) returns ref[T]?

    ; Explizite Kopie
    function copy_at(this, i: size_t) returns T?

    ; Iterator gibt refs
    function get_iterator(this) returns VectorIterator[T]
}

; Nutzung
for item in v:        ; item: ref[T]
    print(*item)      ; Dereferenzieren fuer Wert
```

### Escape-Analyse
```scaly
function bad() returns ref[int]
    let v Vector[int]$()
    v.get(0).unwrap()  ; FEHLER: ref wuerde Region ueberleben

function good() returns int
    let v Vector[int]$()
    *v.get(0).unwrap()  ; OK: Wert kopiert
```

---

## TODO: Features fuer Compiler-Portierung

### Phase 1: Grundlagen (ref[T] + Collections)

- [ ] **ref[T] Typ implementieren**
  - Neuer Typ in Model.h/Plan.h
  - Planner: Escape-Analyse fuer Region-Lifetime
  - Emitter: Als Pointer ohne Ownership emittieren

- [ ] **Vector[T] mit ref[T] API**
  - get() returns ref[T]?
  - copy_at() returns T?
  - Iterator mit ref[T]
  - push/pop bleiben gleich

- [ ] **HashMap[K,V] implementieren**
  - Aus retired/ aktivieren und anpassen
  - get() returns ref[V]?
  - keys()/values() als Iteratoren mit refs
  - Kritisch fuer Symbol-Tabellen

- [ ] **HashSet[T] implementieren**
  - contains() returns bool
  - Iterator mit ref[T]

### Phase 2: Error-Handling

- [ ] **Result[T, E] Typ**
  - Union mit Ok(T) und Err(E)
  - Equivalent zu llvm::Expected<T>

- [ ] **? Operator fuer fruehen Return**
  - `let x = operation()?`
  - Bei Err: return Err
  - Bei Ok: extrahiere Wert

- [ ] **Error-Typen definieren**
  - ModelError, PlannerError, EmitterError
  - Mit Span/Location Info

### Phase 3: String-Operationen

- [ ] **StringRef (borrowed) vs String (owned)**
  - StringRef als ref[String] oder eigener Typ?
  - Lifetime an Region gebunden

- [ ] **String-Methoden erweitern**
  - substr(start, len) returns StringRef
  - contains(needle) returns bool
  - starts_with(prefix) returns bool
  - split(delimiter) returns Iterator

### Phase 4: File I/O

- [ ] **File I/O aus retired/ aktivieren**
  - read_file(path) returns Result[String, FileError]
  - Fuer Source-Files lesen

- [ ] **Path-Operationen**
  - join, parent, filename etc.

### Phase 5: Compiler-Komponenten portieren

- [ ] **Modeler.scaly** (~2100 LOC C++)
  - Kleinste Komponente
  - AST -> Model Transformation
  - Symbol-Tabellen mit HashMap

- [ ] **Planner.scaly** (~8000 LOC C++)
  - Type Inference
  - Monomorphisierung
  - Name Mangling
  - Komplexeste Komponente

- [ ] **Emitter.scaly** (~4500 LOC C++)
  - LLVM IR Generation
  - Braucht LLVM FFI/Bindings
  - Oder: Eigenes IR Format?

---

## C++ -> Scaly Mapping

| C++ Feature | Scaly Equivalent |
|-------------|------------------|
| `std::variant` + `std::visit` | `union` + `choose`/`when` |
| `std::optional<T>` | `Option[T]` / `T?` |
| `std::vector<T>` | `Vector[T]` |
| `std::map<K,V>` | `HashMap[K,V]` |
| `llvm::Expected<T>` | `Result[T, Error]` |
| `llvm::StringRef` | `ref[String]` oder `StringRef` |
| `std::unique_ptr<T>` | Wert mit `$` Lifetime |
| `std::shared_ptr<T>` | `pointer[T]#` (caller region) |
| `std::get_if<T>` | `choose`/`when` mit binding |

---

## Notizen

- pointer[T] bleibt nur fuer Container-interna
- ref[T] ist die sichere API fuer Anwendungscode
- RBMM-Regionen geben uns Lifetime-Tracking "gratis"
- Kein voller Borrow-Checker noetig
