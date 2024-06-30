// swift-tools-version: 5.10
import PackageDescription

let package = Package(
  name: "MethaneKit",
  platforms: [
    .macOS(.v14),
    .visionOS(.v1),
    .iOS(.v17),
    .tvOS(.v17),
    .watchOS(.v10)
  ],
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
        // ----- graphics ---
        "MethaneGraphicsTypes",
        "MethaneGraphicsRHIInterface",
        "MethaneGraphicsRHIBase",
        "MethaneGraphicsRHIMetal",
        "MethaneGraphicsRHIVulkan",
        "MethaneGraphicsRHIDirectX",
        // ----- platform ---
        "MethanePlatformAppView",
        "MethanePlatformUtils",
        // -------- swift ---
        "MethaneKit"
      ]
    ),

    .library(
      name: "taskflow",
      targets: [
        "taskflow"
      ]
    ),

    .library(
      name: "nowide",
      targets: [
        "nowide"
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
      name: "taskflow"
    ),

    .target(
      name: "nowide"
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
      name: "MethanePlatformAppView",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneInstrumentation")
      ],
      path: "Modules/Platform/AppView",
      exclude: Arch.getExcludes(for: .appView),
      publicHeadersPath: "Include",
      cxxSettings: Arch.getCxxSettings(for: .appView)
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
      name: "MethaneGraphicsTypes",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneInstrumentation"),
      ],
      path: "Modules/Graphics/Types",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneGraphicsRHIInterface",
      dependencies: [
        .target(name: "MethanePrimitives"),
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneDataProvider"),
        .target(name: "MethaneDataRangeSet"),
        .target(name: "MethaneDataEvents"),
        .target(name: "MethaneDataPrimitives"),
        .target(name: "MethaneGraphicsTypes"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethanePlatformAppView"),
      ],
      path: "Modules/Graphics/RHI/Interface",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneGraphicsRHIBase",
      dependencies: [
        .target(name: "taskflow"),
        .target(name: "nowide"),
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneDataPrimitives"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethaneGraphicsRHIInterface"),
      ],
      path: "Modules/Graphics/RHI/Base",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneGraphicsRHIMetal",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneDataPrimitives"),
        .target(name: "MethanePlatformUtils"),
        .target(name: "MethanePlatformAppView"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethaneGraphicsRHIBase"),
      ],
      path: "Modules/Graphics/RHI/Metal",
      publicHeadersPath: "Include",
      linkerSettings: [
        .linkedFramework("Foundation", .when(platforms: [.macOS, .iOS, .visionOS, .tvOS, .watchOS])),
        .linkedFramework("QuartzCore", .when(platforms: [.macOS, .iOS, .visionOS, .tvOS, .watchOS])),
        .linkedFramework("CoreVideo", .when(platforms: [.macOS, .iOS, .visionOS, .tvOS, .watchOS])),
        .linkedFramework("Metal", .when(platforms: [.macOS, .iOS, .visionOS, .tvOS, .watchOS])),
        .linkedFramework("AppKit", .when(platforms: [.macOS])),
        .linkedFramework("UIKit", .when(platforms: [.iOS, .visionOS, .tvOS, .watchOS])),
      ]
    ),

    .target(
      name: "MethaneGraphicsRHIVulkan",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneDataPrimitives"),
        .target(name: "MethanePlatformUtils"),
        .target(name: "MethanePlatformAppView"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethaneGraphicsRHIBase"),
      ],
      path: "Modules/Graphics/RHI/Vulkan",
      publicHeadersPath: "Include"
    ),

    .target(
      name: "MethaneGraphicsRHIDirectX",
      dependencies: [
        .target(name: "MethaneDataTypes"),
        .target(name: "MethaneDataPrimitives"),
        .target(name: "MethanePlatformUtils"),
        .target(name: "MethanePlatformAppView"),
        .target(name: "MethaneInstrumentation"),
        .target(name: "MethaneGraphicsRHIBase"),
      ],
      path: "Modules/Graphics/RHI/DirectX",
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
        .target(name: "MethaneGraphicsTypes"),
        .target(name: "MethaneGraphicsRHIInterface"),
        .target(name: "MethaneGraphicsRHIBase"),
        .target(name: "MethaneGraphicsRHIMetal", condition: .when(platforms: [.macOS, .visionOS, .iOS, .tvOS, .watchOS])),
        .target(name: "MethaneGraphicsRHIVulkan", condition: .when(platforms: [.linux])),
        .target(name: "MethaneGraphicsRHIDirectX", condition: .when(platforms: [.windows])),
        .target(name: "MethanePlatformUtils"),
        .target(name: "MethanePlatformAppView"),
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
    case appView = "AppView"
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
      case .appView:
        for os in Arch.OS.allCases.filter({ !Arch.host.contains($0.rawValue.lowercased()) })
        {
          if Arch.host.contains(OS.apple.rawValue.lowercased())
          {
            if Arch.device.contains("macosx")
            {
              excludes.append("Include/Methane/Platform/iOS/AppEnvironment.hh")
              excludes.append("Include/Methane/Platform/iOS/AppViewMetal.hh")
              excludes.append("Sources/Methane/Platform/iOS/AppViewMetal.mm")
            }
            else
            {
              excludes.append("Include/Methane/Platform/MacOS/AppEnvironment.hh")
              excludes.append("Include/Methane/Platform/MacOS/AppViewMetal.hh")
              excludes.append("Sources/Methane/Platform/MacOS/AppViewMetal.mm")
            }

            excludes.append("Include/Methane/Platform/\(os.rawValue)/AppEnvironment.h")
          }
          else
          {
            excludes.append("Sources/Methane/Platform/MacOS/AppViewMetal.mm")
            excludes.append("Include/Methane/Platform/MacOS/AppEnvironment.hh")
            excludes.append("Include/Methane/Platform/MacOS/AppViewMetal.hh")

            excludes.append("Sources/Methane/Platform/iOS/AppViewMetal.mm")
            excludes.append("Include/Methane/Platform/iOS/AppEnvironment.hh")
            excludes.append("Include/Methane/Platform/iOS/AppViewMetal.hh")
          }
        }
    }

    return excludes
  }

  public static func getCxxSettings(for target: MethaneTarget) -> [CXXSetting]
  {
    var settings = [CXXSetting]()

    switch target
    {
      case .appView:
        if Arch.device.contains("macosx")
        {
          settings.append(.define("APPLE_MACOS", to: "1"))
        }
      default:
        break
    }

    return settings
  }
}
