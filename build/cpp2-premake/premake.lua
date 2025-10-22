
-- defines the msbuild includes used to bind in cppfront/cpp2
externalrule "cpp2"
    location "../cpp2-msbuild/"
    display "cppfront compiler"
    fileextension ".cpp2"
    fileextension ".h2"

    propertydefinition {
      name = "FormatColonErrors",
      kind = "boolean"
    }
    propertydefinition {
      name = "StdHandling",
      kind = "string"
    }
    propertydefinition {
      name = "PureCpp2",
      kind = "boolean"
    }
    propertydefinition {
      name = "EmitCleanCpp1",
      kind = "boolean"
    }
    propertydefinition {
      name = "Verbosity",
      kind = "string"
    }
    propertydefinition {
      name = "DisableCppEH",
      kind = "boolean"
    }
    propertydefinition {
      name = "DisableCppRTTI",
      kind = "boolean"
    }


    propertydefinition {
      name = "DispatchToolLocation",
      kind = "string"
    }
