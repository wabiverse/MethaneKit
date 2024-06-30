// swift-tools-version: 5.10
import PackageDescription

let package = Package(
  name: "MethaneKit",
  products: [
    .library(
      name: "MethaneKit",
      targets: [
        // ------- common ---
        "MethanePrimitives",
        "MethaneInstrumentation",
        // --------- data ---
        "MethaneDataPrimitives",
        "MethaneDataRangeSet",
        "MethaneDataTypes",
        "MethaneDataProvider",
        "MethaneDataEvents",
        "MethaneDataAnimation",
        // ----- platform ---
        "MethanePlatformUtils",
        // -------- swift ---
        "MethaneKit"
      ]
    ),

    .library(
      name: "Tracy",
      targets: [
        "Tracy"
      ]
    ),

    .library(
      name: "HLSLpp",
      targets: [
        "magic_enum",
        "platforms",
        "HLSLpp"
      ]
    ),
  ],
  targets: [
    .target(
      name: "fmt",
      publicHeadersPath: "include"
    ),

    .target(
      name: "magic_enum"
    ),

    .target(
      name: "Tracy",
      exclude: [
        "libbacktrace",
        "TracyClient.cpp"
      ],
      publicHeadersPath: "include",
      cxxSettings: [
        .define("TRACY_NO_CALLSTACK", to: "1")
      ]
    ),

    .target(
      name: "platforms"
    ),

    .target(
      name: "HLSLpp",
      dependencies: [
        .target(name: "platforms")
      ],
      exclude: [
        "android"
      ]
    ),

    .target(
      name: "MethanePrimitives",
      dependencies: [
        .target(name: "fmt")
      ],
      path: "Modules/Common/Primitives",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneInstrumentation",
      dependencies: [
        .target(name: "Tracy"),
        .target(name: "MethanePrimitives")
      ],
      path: "Modules/Common/Instrumentation",
      exclude: Arch.getExcludes(for: .instrumentation),
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethanePlatformUtils",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneInstrumentation")
      ],
      path: "Modules/Platform/Utils",
      exclude: Arch.getExcludes(for: .platform),
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneDataPrimitives",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneInstrumentation"),
      ],
      path: "Modules/Data/Primitives",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneDataAnimation",
      dependencies: [
        .target(name: "MethanePrimitives"),
        .target(name: "MethaneInstrumentation")
      ],
      path: "Modules/Data/Animation",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneDataEvents",
      dependencies: [
        .target(name: "MethanePrimitives"),
        .target(name: "MethaneInstrumentation")
      ],
      path: "Modules/Data/Events",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneDataProvider",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethanePlatformUtils")
      ],
      path: "Modules/Data/Provider",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneDataRangeSet",
      dependencies: [
        .target(name: "MethanePrimitives"),
        .target(name: "MethaneInstrumentation")
      ],
      path: "Modules/Data/RangeSet",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneDataTypes",
      dependencies: [
        .target(name: "magic_enum"),
        .target(name: "HLSLpp"),
        .target(name: "MethanePrimitives"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethaneDataRangeSet"),
      ],
      path: "Modules/Data/Types",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneKit",
      dependencies: [
        .target(name: "Tracy"),
        .target(name: "MethanePrimitives"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethaneDataPrimitives"),
        .target(name: "MethaneDataRangeSet"),
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneDataProvider"),
        .target(name: "MethaneDataEvents"),
        .target(name: "MethaneDataAnimation"),
        .target(name: "MethanePlatformUtils"),
      ],
      swiftSettings: [
        .interoperabilityMode(.Cxx)
      ]
    ),

    .executableTarget(
      name: "MethaneKitDemo",
      dependencies: [
        .target(name: "MethaneKit"),
      ],
      swiftSettings: [
        .interoperabilityMode(.Cxx)
      ]
    )
  ],
  cxxLanguageStandard: .cxx17
)

/** ------------------------------------------------
 * Just to tidy up the package configuration above,
 * we define some helper functions and types below.
 * ------------------------------------------------ */
enum Arch
{
  /* -------------------------------------------------------------------------
   * To detect platform triplets, we need to know the OS and CPU architecture.
   * -------------------------------------------------------------------------*/

  #if os(macOS) || os(visionOS) || os(iOS) || os(tvOS) || os(watchOS)
    static let host = "apple"
    #if os(macOS)
      static let device = "macosx"
    #elseif os(visionOS)
      static let device = "xros"
    #elseif os(iOS)
      static let device = "ios"
    #elseif os(tvOS)
      static let device = "tvos"
    #elseif os(watchOS)
      static let device = "watchos"
    #endif /* os(watchOS) */
  #elseif os(Linux) || os(Android) || os(OpenBSD) || os(FreeBSD)
    static let host = "unknown-linux"
    #if os(Android)
      static let device = "android"
    #else /* os(Linux) || os(OpenBSD) || os(FreeBSD) */
      static let device = "gnu"
    #endif /* os(Linux) || os(OpenBSD) || os(FreeBSD) */
  #elseif os(Windows)
    static let host = "windows"
    static let device = "windows"
  #elseif os(Cygwin)
    static let host = "cygwin"
    static let device = "windows"
  #elseif os(WASI)
    static let host = "wasi"
    static let device = "wasi"
  #endif /* os(WASI) */

  /** OS platforms, grouped by family. */
  enum OS: String, CaseIterable
  {
    case apple = "Apple"
    case linux = "Linux"
    case windows = "Windows"

    /** Uppercased version of OS platform (ex. 'Apple', 'Linux', 'Windows'). */
    public static var name: String
    {
      if Arch.host.contains(apple.rawValue.lowercased())
      {
        "Apple"
      }
      else if Arch.host.contains(linux.rawValue.lowercased())
      {
        "Linux"
      }
      else if Arch.host.contains(windows.rawValue.lowercased())
      {
        "Windows"
      }
      else
      {
        ""
      }
    }
  }

  enum MethaneTarget: String
  {
    case instrumentation = "Instrumentation"
    case platform = "Platform"
    case provider = "Provider"
  }

  public static func getExcludes(for target: MethaneTarget) -> [String]
  {
    var excludes = [String]()

    switch target
    {
      case .instrumentation:
        for os in Arch.OS.allCases.filter({ !Arch.host.contains($0.rawValue.lowercased()) })
        {
          Arch.host.contains(OS.apple.rawValue.lowercased())
            ? excludes.append("Sources/Methane/\(os.rawValue)/Instrumentation.cpp")
            : excludes.append("Sources/Methane/\(os.rawValue)/Instrumentation.mm")
        }
      case .platform:
        for os in Arch.OS.allCases.filter({ !Arch.host.contains($0.rawValue.lowercased()) })
        {
          Arch.host.contains(OS.apple.rawValue.lowercased())
            ? excludes.append("Sources/Methane/Platform/\(os.rawValue)/Utils.cpp")
            : excludes.append("Sources/Methane/Platform/\(os.rawValue)/Utils.mm")
        }
      case .provider:
        break
    }

    return excludes
  }
}
