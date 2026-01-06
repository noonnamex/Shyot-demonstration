import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'package:http/http.dart' as http;
import 'package:uuid/uuid.dart';
import '../devices/device_state.dart';
import '../loginScreen/login_screen_notifier.dart';
import '../squares/square_state.dart';
import 'home_screen_state.dart';


/// ***************************************************************
///                             PROVIDER
/// ***************************************************************

/// Provider pre HomeScreenNotifier
/// spravuje všetky stavy pre domovskú obrazovku
final homeScreenProvider = StateNotifierProvider<HomeScreenNotifier, HomeScreenState>((ref) {
  final loginState = ref.watch(loginScreenProvider);  // Sleduje stav prihlasenia
  final token = loginState.token;                     // Získa token používateľa

  return HomeScreenNotifier(token);                   // Vytvorí nový HomeScreenNotifier so získaným tokenom
});


/// Provider pre status zariadenia podľa jeho tokenu
final deviceStatusProvider = Provider.family<bool, String?>((ref, deviceToken) {

  if (deviceToken == null || deviceToken.isEmpty) return true;

  // Sleduje stav na obrazovke Home
  final homeState = ref.watch(homeScreenProvider);

  // Hľadá zariadenie podľa tokenu
  final device = homeState.devices.where((d) => d.token == deviceToken).firstOrNull;

  print('🔍 Device status check: $deviceToken -> ${device?.isOnline ?? false}');
  return device?.isOnline ?? false;
});



/// ***************************************************************
///                             NOTIFIER
/// ***************************************************************

/// Notifier pre HomeScreen
/// Tento notifier spravuje stav obrazovky Home, zodpovedá za správu dashboardov (squares) a ďalších nastavení
class HomeScreenNotifier extends StateNotifier<HomeScreenState> {
  final String? token;
  final _secureStorage = FlutterSecureStorage();    // Na uchovávanie bezpečného tokenu
  final Uuid _uuid = const Uuid();                  // Pre generovanie unikátnych ID

  /// Predvolené hodnoty obrazovky
  HomeScreenNotifier(this.token): super(HomeScreenState(
    squares: [],                                    // Prázdny zoznam štvorcov
    backgroundFrom: Color.fromARGB(255, 0, 0, 0),   // Predvolená farba pozadia (od)
    backgroundTo: Color.fromARGB(255, 0, 20, 0),    // Predvolená farba pozadia (do)
    isDarkMode: true,         // Predvolený režim (tma)
    squareSizeScale: 1,       // Predvolená veľkosť štvorcov
    isEditMode: false,        // Režim úprav je vypnutý
    isSharingMode: false,     // Zdieľanie je vypnuté
    isErasingMode: false,     // Vymazávanie je vypnuté
    selectedSquareId: null,   // Žiadny vybraný štvorec
    isLoading: false,         // Počiatočný stav načítania
    errorMessage: null,       // Počiatočný stav bez chýb
  ));


  /// Pridanie nového dashboardu
  Future<void> addSquare(String name, int iconIndex) async {

    try {
      // Nastaví stav načítania
      state = state.copyWith(isLoading: true, errorMessage: null);

      // Získa token používateľa
      final token = await _getUserToken();
      if (token == null) {
        state = state.copyWith(errorMessage: "No auth token available");
        return;
      }

      // POST request na vytvorenie dashboardu
      final url = Uri.parse('https://shyot.com/dashboards');
      final response = await http.post(
        url,
        headers: {
          'Authorization': 'Bearer $token',     // Autorizačný token
          'Content-Type': 'application/json',   // Typ obsahu
        },
        body: jsonEncode({
          'name': name,
          'iconIndex': iconIndex,               // Index ikony pre štvorec
        }),
      );

      // Kontrola odpovede zo servera
      if (response.statusCode == 200 || response.statusCode == 201) {
        await fetchDashboards();
        print('✅ Dashboard created successfully: $name with icon: $iconIndex');
      } else {
        throw Exception('Failed to create dashboard: ${response.statusCode}');
      }
    } catch (e, stack) {
      print("❌ Error creating dashboard: $e\n$stack");
      state = state.copyWith(
        isLoading: false,
        errorMessage: "Error creating dashboard: ${e.toString()}",
      );
    }
  }


  /// Funkcia na prepnutie režimu úprav
  void toggleEditMode() {
    final newEditMode = !state.isEditMode;

    // Ak sa vypína edit mode, vypni aj erasing mode
    if (!newEditMode && state.isErasingMode) {
      state = state.copyWith(
        isEditMode: false,
        isErasingMode: false,
      );
      print('✏️ Edit mode: false');
      print('🗑️ Erasing mode: false (auto-disabled)');
    } else {
      state = state.copyWith(isEditMode: newEditMode);
      print('✏️ Edit mode: $newEditMode');
    }
  }


  /// Funkcia na prepnutie režimu vymazávania
  void toggleErasingMode() {
    final newErasingMode = !state.isErasingMode;
    state = state.copyWith(isErasingMode: newErasingMode);
    print('🗑️ Erasing mode: $newErasingMode');
  }


  /// Funkcia na editovanie existujúceho dashboardu
  Future<void> editSquare(String squareId, String newName, int newIconIndex) async {
    try {
      print('🎯 editSquare called - id: $squareId, name: $newName, icon: $newIconIndex');

      state = state.copyWith(isLoading: true, errorMessage: null);

      final token = await _getUserToken();
      if (token == null) {
        state = state.copyWith(errorMessage: "No auth token available");
        return;
      }

      // Aktualizovanie stavu pre okamžitú odozvu
      final updatedSquares = state.squares.map((square) {
        if (square.id == squareId) {
          print('🔄 Updating square locally: ${square.name} -> $newName, icon: ${square.iconIndex} -> $newIconIndex');
          return square.copyWith(
            name: newName,
            iconIndex: newIconIndex,
          );
        }
        return square;
      }).toList();

      state = state.copyWith(squares: updatedSquares);
      print('🏠 HomeScreen state updated locally');

      // Uloženie zmien na server
      print('🔄 Saving to server...');
      final url = Uri.parse('https://shyot.com/dashboards/$squareId');
      final response = await http.patch(
        url,
        headers: {
          'Authorization': 'Bearer $token',
          'Content-Type': 'application/json',
        },
        body: jsonEncode({
          'newName': newName,
          'iconIndex': newIconIndex,
        }),
      );

      print('📨 Server response status: ${response.statusCode}');

      if (response.statusCode == 200) {
        print('✅ Square updated successfully on server');
        // Opätovné načítanie dashboardov
        await fetchDashboards();
      } else {
        print('❌ Failed to update square on server: ${response.statusCode}');
        throw Exception('Failed to update square: ${response.statusCode}');
      }

    } catch (e, stack) {
      print("❌ Error updating square: $e\n$stack");
      state = state.copyWith(
        errorMessage: "Error updating square: ${e.toString()}",
      );

      // Revert lokálnych zmien v prípade chyby
      await fetchDashboards();
    } finally {
      if (mounted) {
        state = state.copyWith(isLoading: false);
      }
    }
  }


  /// Funkcia na vymazanie štvorca
  Future<void> deleteSquare(String squareId) async {
    print('🗑️ deleteSquare called for: $squareId');

    // Uloženie predchádzajúceho stavu štvorcov pre prípadný revert
    final previousSquares = state.squares;

    // Odstránenie štvorca lokálne
    final updatedSquares =
    previousSquares.where((square) => square.id != squareId).toList();

    state = state.copyWith(squares: updatedSquares);
    print('🗑️ Square $squareId deleted locally');

    try {
      state = state.copyWith(isLoading: true, errorMessage: null);

      final token = await _getUserToken();
      if (token == null) {
        print('❌ No auth token available, reverting delete');
        state = state.copyWith(
          squares: previousSquares,
          errorMessage: "No auth token available",
          isLoading: false,
        );
        return;
      }

      final url = Uri.parse('https://shyot.com/dashboards/$squareId');
      print('🌐 Sending DELETE to: $url');

      final response = await http.delete(
        url,
        headers: {
          'Authorization': 'Bearer $token',
          'Content-Type': 'application/json',
        },
      );

      print('📨 DELETE response status: ${response.statusCode}');
      print('📨 DELETE response body: ${response.body}');

      if (response.statusCode == 200 || response.statusCode == 204) {
        print('✅ Square $squareId deleted successfully on server');
        await fetchDashboards();   // Načítanie nového zoznamu dashboardov
      } else {
        print('❌ Failed to delete square on server: ${response.statusCode}');
        state = state.copyWith(
          squares: previousSquares,   // Revert na predchádzajúci stav
          errorMessage: 'Failed to delete square: ${response.statusCode}',
        );
        await fetchDashboards();
      }
    } catch (e, stack) {
      print("❌ Error deleting square: $e\n$stack");
      // revert lokálne + refetch
      state = state.copyWith(
        squares: previousSquares,
        errorMessage: "Error deleting square: ${e.toString()}",
      );
      await fetchDashboards();
    } finally {
      if (mounted) {
        state = state.copyWith(isLoading: false);
      }
    }
  }


  /// Uloženie indexu ikony do API pre konkrétny dashboard
  static Future<void> saveIconIndexToApi(String dashboardId, int iconIndex, String? token) async {
    try {
      if (token == null) {
        print('❌ No token available for saving icon index');
        return;
      }

      // Telo požiadavky (payload) s novým indexom ikony
      final requestBody = {
        "iconIndex": iconIndex,
      };

      print('🔄 Ukladám icon index: $dashboardId -> $iconIndex');
      print('📦 Request body: ${jsonEncode(requestBody)}');

      // URL na konkrétny dashboard, na ktorý chceme poslať požiadavku
      final url = Uri.parse('https://shyot.com/dashboards/$dashboardId');
      final response = await http.patch(
        url,
        headers: {
          'Authorization': 'Bearer $token',     // Posiela autorizáciu cez token
          'Content-Type': 'application/json',   // Určuje typ obsahu ako JSON
        },
        body: jsonEncode(requestBody),
      );

      // Kontrola odpovede zo servera
      print('📨 Response status: ${response.statusCode}');
      print('📨 Response body: ${response.body}');

      // Ak je odpoveď úspešná, uloží nový index ikony
      if (response.statusCode == 200) {
        print('✅ Icon index saved to API: $dashboardId -> $iconIndex');
      } else {
        print('❌ Failed to save icon index: ${response.statusCode}');
      }
    } catch (e) {
      print('❌ Error saving icon index: $e');
    }
  }


  /// Funkcia na načítanie zariadení používateľa
  Future<void> fetchUserDevices() async {
    try {
      print('🔄 STARTING fetchUserDevices...');

      // Kontrola, či je widget ešte na obrazovke (je to potrebné pri asynchrónnych operáciách)
      if (!mounted) {
        print('⚠️ HomeScreenNotifier already disposed, skipping fetchUserDevices');
        return;
      }

      // Nastavenie stavu načítania
      state = state.copyWith(isLoading: true, errorMessage: null);

      // Získanie tokenu používateľa
      final token = await _getUserToken();
      if (token == null) {
        print('❌ No token available for fetchUserDevices');
        if (mounted) {
          state = state.copyWith(errorMessage: "No auth token available");
        }
        return;
      }

      // URL na získanie zariadení používateľa
      final url = Uri.parse('https://shyot.com/devices/my-devices');
      print('🌐 Fetching devices from: $url');

      final response = await http.get(
        url,
        headers: {
          'Authorization': 'Bearer $token',   // Posiela autorizáciu cez token
          'Content-Type': 'application/json', // Určuje typ obsahu ako JSON
        },
      );

      print('📡 Devices API response status: ${response.statusCode}');

      // Kontrola, či odpoveď zo servera je úspešná (status 200)
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);       // Dekóduje JSON odpoveď
        final devicesData = data['data']['devices'] as List<dynamic>;

        print("✅ Devices fetched: ${devicesData.length}");

        // DEBUG - vypíš všetky zariadenia a ich online stav
        for (var device in devicesData) {
          print('📱 Device: ${device['name']} (${device['token']}) - Online: ${device['isOnline']}');
        }

        // KONTROLA PRED AKTUALIZÁCIOU STAVU
        if (!mounted) {
          print('⚠️ HomeScreenNotifier disposed during fetchUserDevices');
          return;
        }

        final devices = devicesData.map((device) {
          final apiId = device['id']?.toString() ?? '';

          return DeviceState(
            id: apiId,
            token: device['token'] ?? '',
            name: device['name'] ?? 'Unnamed Device',
            description: device['description'] ?? '',
            isActive: device['isActive'] ?? true,
            isOnline: device['isOnline'] ?? false,
            lastSeen: device['lastSeen'] != null
                ? DateTime.parse(device['lastSeen'])
                : DateTime.now(),
            isShared: device['isShared'] ?? false,
            color: _parseColor(device['color']),
            deviceData: _convertPinsToDeviceData(device['currentPins'] ?? {}),
          );
        }).toList();

        // FINÁLNA KONTROLA
        if (mounted) {
          state = state.copyWith(
            devices: devices,     // Uloží načítané zariadenia do stavu
            isLoading: false,     // Nastaví načítanie na false
            errorMessage: null,
          );
        }

      } else {
        print('❌ Devices API error: ${response.statusCode} - ${response.body}');
        throw Exception('Failed to fetch devices: ${response.statusCode}');
      }
    } catch (e, stack) {
      print("❌ Error fetching devices: $e\n$stack");
      if (mounted) {
        state = state.copyWith(
          isLoading: false,
          errorMessage: "Error loading devices: ${e.toString()}",
        );
      }
    }
  }


  /// POMOCNÁ FUNKCIA: Konverzia currentPins na deviceData formát
  List<Map<String, dynamic>> _convertPinsToDeviceData(Map<String, dynamic> pins) {
    return pins.entries.map((entry) => {
      'pin': entry.key,       // pin
      'value': entry.value,   // hodnota
      'timestamp': DateTime.now().toIso8601String(),    // Timestamp
    }).toList();
  }


  /// POMOCNÁ FUNKCIA: Parsovanie farby z hex kódu na Color objekt
  Color _parseColor(String? colorHex) {
    if (colorHex == null || colorHex.isEmpty) {
      return Colors.blue; // Ak je farba neplatná alebo chýba, nastaví sa predvolená modrá
    }

    try {
      final hexCode = colorHex.replaceFirst('#', '');   // Odstráni "#" zo začiatku hex kódu
      return Color(int.parse('0xFF$hexCode'));          // Vytvorí Color objekt zo hex kódu
    } catch (e) {
      return Colors.blue;   // Ak sa vyskytne chyba pri parsovaní, vráti predvolenú modrú farbu
    }
  }


  /// Funkcia na získanie všetkých dashboardov zo servera
  Future<void> fetchDashboards() async {
    try {
      // Kontrola, či je widget ešte aktívny. Ak nie, ukončí funkciu.
      if (!mounted) {
        print('⚠️ HomeScreenNotifier already disposed, skipping fetchDashboards');
        return;
      }

      // Vytvorenie URL pre GET požiadavku na získanie všetkých dashboardov
      final url = Uri.parse('https://shyot.com/dashboards');

      // Nastavenie stavu aplikácie na "loading", aby sa používateľ informoval, že sa načítavajú dáta
      state = state.copyWith(isLoading: true, errorMessage: null);

      // Získanie tokenu používateľa zo zabezpečeného úložiska
      final token = await _getUserToken();

      // Ak nie je token dostupný, nastaví stav chyby a ukončí funkciu
      if (token == null) {
        if (mounted) {
          state = state.copyWith(errorMessage: "No auth token available");
        }
        return;
      }

      // Vykonanie GET požiadavky na server s autorizáciou cez token
      final response = await http.get(
        url,
        headers: {
          'Authorization': 'Bearer $token',     // Posiela autorizáciu cez token
          'Content-Type': 'application/json',   // Určuje typ obsahu ako JSON
        },
      );

      // Ak je odpoveď zo servera úspešná
      if (response.statusCode == 200) {
        final data = jsonDecode(response.body);
        final dashboards = data['data'] as List<dynamic>;

        print("✅ Dashboards fetched: ${dashboards.length}");

        // Mapa zo získaných dát na objekty `SquareState`, ktoré reprezentujú dashboardy
        final squares = dashboards.map((d) {
          final apiId = d['_id']?.toString() ?? '';

          // 🎯 PRIDAJ DEBUG LOG PRE ICON INDEX
          final iconIndex = d['iconIndex'] ?? 0;
          print('📦 Dashboard: ${d['name']} - iconIndex: $iconIndex');

          return SquareState(
            id: apiId,
            name: d['name'] ?? 'Unnamed',         // Názov dashboardu, ak nie je prítomný, nastavíme 'Unnamed'
            x: 0.0,                               // Počiatočné hodnoty pre pozíciu
            y: 0.0,
            width: 5,                             // Predvolená šírka
            height: 5,                            // Predvolená výška
            isShared: false,                      // Predvolená hodnota, či je zdieľaný (false)
            color: Colors.teal,                   // Predvolená farba
            iconIndex: iconIndex,                 // Priraďuje skutočný `iconIndex` z API
          );
        }).toList();    // Konvertujem zoznam na list objektov `SquareState`

        // Ak je widget stále aktívny, aktualizuje stav s načítanými dashboardmi
        if (mounted) {
          state = state.copyWith(
            squares: squares,  // Uloží načítané dashboardy
            isLoading: false,   // Nastaví stav načítania na false
            errorMessage: null, // Zruším predchádzajúcu chybu
          );
        }

        print('🎯 Successfully loaded ${squares.length} squares');
      } else {
        throw Exception('Failed to fetch dashboards: ${response.statusCode}');
      }
    } catch (e, stack) {
      print("❌ Error fetching dashboards: $e\n$stack");
      if (mounted) {
        state = state.copyWith(
          isLoading: false,
          errorMessage: e.toString(),
        );
      }
    }
  }


  /// Pomocná funkcia na získanie používateľského tokenu z bezpečného úložiska
  Future<String?> _getUserToken() async {
    try {
      final token = await _secureStorage.read(key: 'token');       // Číta token z bezpečného úložiska
      if (token == null) {
        print('❌ Token not found in Secure Storage');             // Ak token neexistuje, vypíše chybu
        return null;
      }
      print('✅ Token retrieved: ${token.substring(0, 20)}...');  // Vypíše prvých 20 znakov tokenu pre potvrdenie
      return token;  // Vráti token
    } catch (e) {
      print('❌ Error getting token: $e');  // Ak sa vyskytla chyba pri získavaní tokenu, vypíše ju
      return null;  // Ak je chyba, vráti null
    }
  }


  /// Funkcia na kontrolu, či je zariadenie online
  bool isDeviceOnline(String deviceToken) {
    // Hľadá zariadenie s daným tokenom v stave
    final device = state.devices.where((d) => d.token == deviceToken).firstOrNull;
    return device?.isOnline ?? false;  // Ak zariadenie existuje, vráti jeho stav online, inak false
  }

  /// Funkcia na získanie zariadenia podľa tokenu
  DeviceState? getDeviceByToken(String deviceToken) {
    // Hľadá zariadenie s daným tokenom v stave a vracia ho
    return state.devices.where((d) => d.token == deviceToken).firstOrNull;
  }



}