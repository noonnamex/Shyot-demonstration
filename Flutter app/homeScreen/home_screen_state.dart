import 'package:flutter/material.dart';
import 'package:freezed_annotation/freezed_annotation.dart';
import 'package:shyot/squares/square_state.dart';
import '../devices/device_state.dart';


part 'home_screen_state.freezed.dart';

@freezed
class HomeScreenState with _$HomeScreenState {
  const factory HomeScreenState({

    @Default([]) List<SquareState> squares,    // Dashboardy (len metadáta – napr. pozície, poradie, zdieľanie)
    @Default([]) List<DeviceState> devices,

    required Color backgroundFrom,              // Farby pozadia
    required Color backgroundTo,
    required bool isDarkMode,                   // Dark mód

    required double squareSizeScale,            // Globálna veľkosť dashboardov (1.0 = normálna veľkosť)

    required bool isEditMode,                   // Či je používateľ v editačnom móde
    required bool isErasingMode,                // Či je používateľ v editačnom móde
    required bool isSharingMode,                // Ak chce zdieľať

    String? selectedSquareId,                   // ID vybraného dashboardu (ak je niečo vybrané)

    // Na budúcnosť
    @Default(false) bool isLoading,             // Loader
    String? errorMessage,                       // Ak sa niečo pokazí
  }) = _HomeScreenState;
}