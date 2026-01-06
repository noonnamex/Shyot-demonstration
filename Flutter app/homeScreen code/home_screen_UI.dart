import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:shyot/utilities/appbar_UI.dart';
import '../deviceScreen/device_screen_UI.dart';
import '../grid/grid_UI.dart';
import '../grid/grid_notifier.dart';
import '../squares/square_UI.dart';
import '../utilities/bottomAppBar_UI.dart';
import '../utilities/lifecycle_service.dart';
import 'home_screen_notifier.dart';
import '../utilities/snow.dart';

/// ***************************************************************
///                             HOMESCREEN UI
/// ***************************************************************

/// Hlavná obrazovka domovskej stránky, ktorá využíva Riverpod pre správu stavu
class HomeScreen extends ConsumerStatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  ConsumerState<HomeScreen> createState() => _HomeScreenState();
}



class _HomeScreenState extends ConsumerState<HomeScreen> {

  // Deklarácia pre callback funkciu pri obnovení aplikácie (resume)
  VoidCallback? _resumeCallback;

  @override
  void initState() {

    super.initState();

    // Registrovanie životného cyklu a načítanie dashboardov a zariadení pri štarte obrazovky

    _registerLifecycle();

    // Načítanie dát (dashboardov a zariadení) po prvej vykreslení obrazovky
    Future.microtask(() {
      ref.read(homeScreenProvider.notifier).fetchDashboards();  // Načítanie dashboardov
      ref.read(homeScreenProvider.notifier).fetchUserDevices(); // Načítanie používateľov
    });
  }


  /// Funkcia na registráciu listenera pre obnovenie aplikácie (resumed)
  void _registerLifecycle() {
    final lifecycleService = ref.read(lifecycleServiceProvider);  // Získanie služby pre životný cyklus

    // Callback, ktorý sa spustí, keď sa aplikácia vráti do popredia (resume)
    _resumeCallback = () {
      if (mounted) {
        print('🔄 Refreshing home screen...');
        ref.read(homeScreenProvider.notifier).fetchDashboards();  // Obnoví dashboardy
      }
    };

    // listener, ktorý bude reagovať na resume (obnovenie aplikácie)
    lifecycleService.addOnResumeListener(_resumeCallback!);
  }


  @override
  void dispose() {
    // Dôležité: Odstránenie listenera pri dispose obrazovky
    if (_resumeCallback != null) {
      ref.read(lifecycleServiceProvider).removeOnResumeListener(_resumeCallback!);
    }
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final homeState = ref.watch(homeScreenProvider);              // Sledovanie aktuálneho stavu home screen
    final gridState = ref.read(gridProvider);                     // Získanie stavu grid
    final gridPixelSize = gridState.getGridPixelSize(context);    // Výpočet veľkosti pixelu mriežky
    final notifier = ref.read(homeScreenProvider.notifier);       // Získanie notifiera pre HomeScreen

    return Scaffold(
      appBar: buildAppBar(title: 'Dashboard screen'),             // Priradenie AppBar
      bottomNavigationBar: buildBottomBar(                        // Pridanie bottombar s tlačidlami
        title: 'title',
        onAddPressed: () => _showAddSquareModal(context, ref.read(homeScreenProvider.notifier)),
        onDevicesPressed: () => Navigator.push(context, MaterialPageRoute(builder: (context) => const DeviceScreen())),
        onEditPressed: () => ref.read(homeScreenProvider.notifier).toggleEditMode(),
        onDeletePressed: () => ref.read(homeScreenProvider.notifier).toggleErasingMode(),
        isEditMode: homeState.isEditMode,
        isErasingMode: homeState.isErasingMode,
      ),

      body: Container(
        decoration: paintBackground(),  // pozadie
        child: Stack(
          children: [

            SnowWidget(snowflakeCount: 80), // Vykreslenie snehu na pozadí

            if (homeState.isEditMode) GridOverlay(),  // Zobrazí mriežku, ak je režim editMode

            // pozícia UI dashboardov
            for (int i = 0; i < homeState.squares.length; i++)
              Positioned(
                left: gridPixelSize + (i % 2) * (gridPixelSize * 8 + gridPixelSize * 2), // 2 karty vedľa seba
                top: gridPixelSize + (i ~/ 2) * (gridPixelSize * 8 + 16), // Riadky
                child: Square(
                  squareId: homeState.squares[i].id,
                  squareName: homeState.squares[i].name,
                  iconIndex: homeState.squares[i].iconIndex,
                  gridState: gridState,
                  onEdit: homeState.isEditMode
                      ? (newName, newIconIndex) {
                    notifier.editSquare(homeState.squares[i].id, newName, newIconIndex);
                  }
                      : null,
                  onDelete: homeState.isErasingMode
                      ? () => _confirmDeleteDashboard(
                    context,
                    notifier,
                    homeState.squares[i].id,
                    homeState.squares[i].name,
                  )
                      : null,
                ),
              ),
          ],
        ),
      ),
    );
  }

  /// Funkcia pre okno potvrdenia vymazania dashboardu
  void _confirmDeleteDashboard(
      BuildContext context,
      HomeScreenNotifier notifier,
      String squareId,
      String squareName,
      ) {
    showDialog(

      context: context,
      builder: (context) {

        // okno s nápisom
        return AlertDialog(
          title: const Text('Odstrániť dashboard?'),
          content: Text('Naozaj chcete odstrániť dashboard "$squareName"?'),

          actions: [

            // možnosť zrušiť
            TextButton(
              onPressed: () {
                Navigator.of(context).pop(); // zavrie dialog
              },
              child: const Text('Zrušiť'),
            ),

            // možnosť odstrániť
            TextButton(
              onPressed: () {
                Navigator.of(context).pop(); // zavrie dialog
                notifier.deleteSquare(squareId); // až potom maže
              },
              style: TextButton.styleFrom(
                foregroundColor: Colors.red,
              ),
              child: const Text('Odstrániť'),
            ),
          ],
        );
      },
    );
  }

  /// Funkcia na zobrazenie modálneho okna pre pridanie nového štvorca
  void _showAddSquareModal(BuildContext context, HomeScreenNotifier notifier) {
    showDialog(
      context: context,
      builder: (context) => AddSquareModal(
        onSave: (String name, int iconIndex) => notifier.addSquare(name,iconIndex),
      ),
    );
  }
}


/// Funkcia na vytvorenie pozadia s obrázkom (santa obrázok)
BoxDecoration paintBackground() {
  return BoxDecoration(
    image: DecorationImage(
      image: AssetImage('assets/backgrounds/santa.png'),
      fit: BoxFit.cover, // alebo BoxFit.fill/contain podľa potreby
      colorFilter: ColorFilter.mode(
        Colors.black.withOpacity(0.5), // ← 20% stmavenie
        BlendMode.darken,
      ),
    ),
  );
  /*
    BoxDecoration(
    gradient: LinearGradient(
      colors: [const Color(0xFF121212), const Color(0xFF373737)],
      begin: Alignment.topLeft,
      end: Alignment.bottomRight,
    ),
  );*/
}