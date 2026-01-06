// coverage:ignore-file
// GENERATED CODE - DO NOT MODIFY BY HAND
// ignore_for_file: type=lint
// ignore_for_file: unused_element, deprecated_member_use, deprecated_member_use_from_same_package, use_function_type_syntax_for_parameters, unnecessary_const, avoid_init_to_null, invalid_override_different_default_values_named, prefer_expression_function_bodies, annotate_overrides, invalid_annotation_target, unnecessary_question_mark

part of 'home_screen_state.dart';

// **************************************************************************
// FreezedGenerator
// **************************************************************************

T _$identity<T>(T value) => value;

final _privateConstructorUsedError = UnsupportedError(
  'It seems like you constructed your class using `MyClass._()`. This constructor is only meant to be used by freezed and you are not supposed to need it nor use it.\nPlease check the documentation here for more information: https://github.com/rrousselGit/freezed#adding-getters-and-methods-to-our-models',
);

/// @nodoc
mixin _$HomeScreenState {
  List<SquareState> get squares =>
      throw _privateConstructorUsedError; // Dashboardy (len metadáta – napr. pozície, poradie, zdieľanie)
  List<DeviceState> get devices => throw _privateConstructorUsedError;
  Color get backgroundFrom =>
      throw _privateConstructorUsedError; // Farby pozadia
  Color get backgroundTo => throw _privateConstructorUsedError;
  bool get isDarkMode => throw _privateConstructorUsedError; // Mód
  double get squareSizeScale =>
      throw _privateConstructorUsedError; // Globálna veľkosť dashboardov (1.0 = normálna veľkosť)
  bool get isEditMode =>
      throw _privateConstructorUsedError; // Či je používateľ v editačnom móde
  bool get isErasingMode =>
      throw _privateConstructorUsedError; // Či je používateľ v editačnom móde
  bool get isSharingMode =>
      throw _privateConstructorUsedError; // Ak chce zdieľať
  String? get selectedSquareId =>
      throw _privateConstructorUsedError; // ID vybraného dashboardu (ak je niečo vybrané)
  // Na budúcnosť
  bool get isLoading => throw _privateConstructorUsedError; // Loader
  String? get errorMessage => throw _privateConstructorUsedError;

  /// Create a copy of HomeScreenState
  /// with the given fields replaced by the non-null parameter values.
  @JsonKey(includeFromJson: false, includeToJson: false)
  $HomeScreenStateCopyWith<HomeScreenState> get copyWith =>
      throw _privateConstructorUsedError;
}

/// @nodoc
abstract class $HomeScreenStateCopyWith<$Res> {
  factory $HomeScreenStateCopyWith(
    HomeScreenState value,
    $Res Function(HomeScreenState) then,
  ) = _$HomeScreenStateCopyWithImpl<$Res, HomeScreenState>;
  @useResult
  $Res call({
    List<SquareState> squares,
    List<DeviceState> devices,
    Color backgroundFrom,
    Color backgroundTo,
    bool isDarkMode,
    double squareSizeScale,
    bool isEditMode,
    bool isErasingMode,
    bool isSharingMode,
    String? selectedSquareId,
    bool isLoading,
    String? errorMessage,
  });
}

/// @nodoc
class _$HomeScreenStateCopyWithImpl<$Res, $Val extends HomeScreenState>
    implements $HomeScreenStateCopyWith<$Res> {
  _$HomeScreenStateCopyWithImpl(this._value, this._then);

  // ignore: unused_field
  final $Val _value;
  // ignore: unused_field
  final $Res Function($Val) _then;

  /// Create a copy of HomeScreenState
  /// with the given fields replaced by the non-null parameter values.
  @pragma('vm:prefer-inline')
  @override
  $Res call({
    Object? squares = null,
    Object? devices = null,
    Object? backgroundFrom = null,
    Object? backgroundTo = null,
    Object? isDarkMode = null,
    Object? squareSizeScale = null,
    Object? isEditMode = null,
    Object? isErasingMode = null,
    Object? isSharingMode = null,
    Object? selectedSquareId = freezed,
    Object? isLoading = null,
    Object? errorMessage = freezed,
  }) {
    return _then(
      _value.copyWith(
            squares:
                null == squares
                    ? _value.squares
                    : squares // ignore: cast_nullable_to_non_nullable
                        as List<SquareState>,
            devices:
                null == devices
                    ? _value.devices
                    : devices // ignore: cast_nullable_to_non_nullable
                        as List<DeviceState>,
            backgroundFrom:
                null == backgroundFrom
                    ? _value.backgroundFrom
                    : backgroundFrom // ignore: cast_nullable_to_non_nullable
                        as Color,
            backgroundTo:
                null == backgroundTo
                    ? _value.backgroundTo
                    : backgroundTo // ignore: cast_nullable_to_non_nullable
                        as Color,
            isDarkMode:
                null == isDarkMode
                    ? _value.isDarkMode
                    : isDarkMode // ignore: cast_nullable_to_non_nullable
                        as bool,
            squareSizeScale:
                null == squareSizeScale
                    ? _value.squareSizeScale
                    : squareSizeScale // ignore: cast_nullable_to_non_nullable
                        as double,
            isEditMode:
                null == isEditMode
                    ? _value.isEditMode
                    : isEditMode // ignore: cast_nullable_to_non_nullable
                        as bool,
            isErasingMode:
                null == isErasingMode
                    ? _value.isErasingMode
                    : isErasingMode // ignore: cast_nullable_to_non_nullable
                        as bool,
            isSharingMode:
                null == isSharingMode
                    ? _value.isSharingMode
                    : isSharingMode // ignore: cast_nullable_to_non_nullable
                        as bool,
            selectedSquareId:
                freezed == selectedSquareId
                    ? _value.selectedSquareId
                    : selectedSquareId // ignore: cast_nullable_to_non_nullable
                        as String?,
            isLoading:
                null == isLoading
                    ? _value.isLoading
                    : isLoading // ignore: cast_nullable_to_non_nullable
                        as bool,
            errorMessage:
                freezed == errorMessage
                    ? _value.errorMessage
                    : errorMessage // ignore: cast_nullable_to_non_nullable
                        as String?,
          )
          as $Val,
    );
  }
}

/// @nodoc
abstract class _$$HomeScreenStateImplCopyWith<$Res>
    implements $HomeScreenStateCopyWith<$Res> {
  factory _$$HomeScreenStateImplCopyWith(
    _$HomeScreenStateImpl value,
    $Res Function(_$HomeScreenStateImpl) then,
  ) = __$$HomeScreenStateImplCopyWithImpl<$Res>;
  @override
  @useResult
  $Res call({
    List<SquareState> squares,
    List<DeviceState> devices,
    Color backgroundFrom,
    Color backgroundTo,
    bool isDarkMode,
    double squareSizeScale,
    bool isEditMode,
    bool isErasingMode,
    bool isSharingMode,
    String? selectedSquareId,
    bool isLoading,
    String? errorMessage,
  });
}

/// @nodoc
class __$$HomeScreenStateImplCopyWithImpl<$Res>
    extends _$HomeScreenStateCopyWithImpl<$Res, _$HomeScreenStateImpl>
    implements _$$HomeScreenStateImplCopyWith<$Res> {
  __$$HomeScreenStateImplCopyWithImpl(
    _$HomeScreenStateImpl _value,
    $Res Function(_$HomeScreenStateImpl) _then,
  ) : super(_value, _then);

  /// Create a copy of HomeScreenState
  /// with the given fields replaced by the non-null parameter values.
  @pragma('vm:prefer-inline')
  @override
  $Res call({
    Object? squares = null,
    Object? devices = null,
    Object? backgroundFrom = null,
    Object? backgroundTo = null,
    Object? isDarkMode = null,
    Object? squareSizeScale = null,
    Object? isEditMode = null,
    Object? isErasingMode = null,
    Object? isSharingMode = null,
    Object? selectedSquareId = freezed,
    Object? isLoading = null,
    Object? errorMessage = freezed,
  }) {
    return _then(
      _$HomeScreenStateImpl(
        squares:
            null == squares
                ? _value._squares
                : squares // ignore: cast_nullable_to_non_nullable
                    as List<SquareState>,
        devices:
            null == devices
                ? _value._devices
                : devices // ignore: cast_nullable_to_non_nullable
                    as List<DeviceState>,
        backgroundFrom:
            null == backgroundFrom
                ? _value.backgroundFrom
                : backgroundFrom // ignore: cast_nullable_to_non_nullable
                    as Color,
        backgroundTo:
            null == backgroundTo
                ? _value.backgroundTo
                : backgroundTo // ignore: cast_nullable_to_non_nullable
                    as Color,
        isDarkMode:
            null == isDarkMode
                ? _value.isDarkMode
                : isDarkMode // ignore: cast_nullable_to_non_nullable
                    as bool,
        squareSizeScale:
            null == squareSizeScale
                ? _value.squareSizeScale
                : squareSizeScale // ignore: cast_nullable_to_non_nullable
                    as double,
        isEditMode:
            null == isEditMode
                ? _value.isEditMode
                : isEditMode // ignore: cast_nullable_to_non_nullable
                    as bool,
        isErasingMode:
            null == isErasingMode
                ? _value.isErasingMode
                : isErasingMode // ignore: cast_nullable_to_non_nullable
                    as bool,
        isSharingMode:
            null == isSharingMode
                ? _value.isSharingMode
                : isSharingMode // ignore: cast_nullable_to_non_nullable
                    as bool,
        selectedSquareId:
            freezed == selectedSquareId
                ? _value.selectedSquareId
                : selectedSquareId // ignore: cast_nullable_to_non_nullable
                    as String?,
        isLoading:
            null == isLoading
                ? _value.isLoading
                : isLoading // ignore: cast_nullable_to_non_nullable
                    as bool,
        errorMessage:
            freezed == errorMessage
                ? _value.errorMessage
                : errorMessage // ignore: cast_nullable_to_non_nullable
                    as String?,
      ),
    );
  }
}

/// @nodoc

class _$HomeScreenStateImpl implements _HomeScreenState {
  const _$HomeScreenStateImpl({
    final List<SquareState> squares = const [],
    final List<DeviceState> devices = const [],
    required this.backgroundFrom,
    required this.backgroundTo,
    required this.isDarkMode,
    required this.squareSizeScale,
    required this.isEditMode,
    required this.isErasingMode,
    required this.isSharingMode,
    this.selectedSquareId,
    this.isLoading = false,
    this.errorMessage,
  }) : _squares = squares,
       _devices = devices;

  final List<SquareState> _squares;
  @override
  @JsonKey()
  List<SquareState> get squares {
    if (_squares is EqualUnmodifiableListView) return _squares;
    // ignore: implicit_dynamic_type
    return EqualUnmodifiableListView(_squares);
  }

  // Dashboardy (len metadáta – napr. pozície, poradie, zdieľanie)
  final List<DeviceState> _devices;
  // Dashboardy (len metadáta – napr. pozície, poradie, zdieľanie)
  @override
  @JsonKey()
  List<DeviceState> get devices {
    if (_devices is EqualUnmodifiableListView) return _devices;
    // ignore: implicit_dynamic_type
    return EqualUnmodifiableListView(_devices);
  }

  @override
  final Color backgroundFrom;
  // Farby pozadia
  @override
  final Color backgroundTo;
  @override
  final bool isDarkMode;
  // Mód
  @override
  final double squareSizeScale;
  // Globálna veľkosť dashboardov (1.0 = normálna veľkosť)
  @override
  final bool isEditMode;
  // Či je používateľ v editačnom móde
  @override
  final bool isErasingMode;
  // Či je používateľ v editačnom móde
  @override
  final bool isSharingMode;
  // Ak chce zdieľať
  @override
  final String? selectedSquareId;
  // ID vybraného dashboardu (ak je niečo vybrané)
  // Na budúcnosť
  @override
  @JsonKey()
  final bool isLoading;
  // Loader
  @override
  final String? errorMessage;

  @override
  String toString() {
    return 'HomeScreenState(squares: $squares, devices: $devices, backgroundFrom: $backgroundFrom, backgroundTo: $backgroundTo, isDarkMode: $isDarkMode, squareSizeScale: $squareSizeScale, isEditMode: $isEditMode, isErasingMode: $isErasingMode, isSharingMode: $isSharingMode, selectedSquareId: $selectedSquareId, isLoading: $isLoading, errorMessage: $errorMessage)';
  }

  @override
  bool operator ==(Object other) {
    return identical(this, other) ||
        (other.runtimeType == runtimeType &&
            other is _$HomeScreenStateImpl &&
            const DeepCollectionEquality().equals(other._squares, _squares) &&
            const DeepCollectionEquality().equals(other._devices, _devices) &&
            (identical(other.backgroundFrom, backgroundFrom) ||
                other.backgroundFrom == backgroundFrom) &&
            (identical(other.backgroundTo, backgroundTo) ||
                other.backgroundTo == backgroundTo) &&
            (identical(other.isDarkMode, isDarkMode) ||
                other.isDarkMode == isDarkMode) &&
            (identical(other.squareSizeScale, squareSizeScale) ||
                other.squareSizeScale == squareSizeScale) &&
            (identical(other.isEditMode, isEditMode) ||
                other.isEditMode == isEditMode) &&
            (identical(other.isErasingMode, isErasingMode) ||
                other.isErasingMode == isErasingMode) &&
            (identical(other.isSharingMode, isSharingMode) ||
                other.isSharingMode == isSharingMode) &&
            (identical(other.selectedSquareId, selectedSquareId) ||
                other.selectedSquareId == selectedSquareId) &&
            (identical(other.isLoading, isLoading) ||
                other.isLoading == isLoading) &&
            (identical(other.errorMessage, errorMessage) ||
                other.errorMessage == errorMessage));
  }

  @override
  int get hashCode => Object.hash(
    runtimeType,
    const DeepCollectionEquality().hash(_squares),
    const DeepCollectionEquality().hash(_devices),
    backgroundFrom,
    backgroundTo,
    isDarkMode,
    squareSizeScale,
    isEditMode,
    isErasingMode,
    isSharingMode,
    selectedSquareId,
    isLoading,
    errorMessage,
  );

  /// Create a copy of HomeScreenState
  /// with the given fields replaced by the non-null parameter values.
  @JsonKey(includeFromJson: false, includeToJson: false)
  @override
  @pragma('vm:prefer-inline')
  _$$HomeScreenStateImplCopyWith<_$HomeScreenStateImpl> get copyWith =>
      __$$HomeScreenStateImplCopyWithImpl<_$HomeScreenStateImpl>(
        this,
        _$identity,
      );
}

abstract class _HomeScreenState implements HomeScreenState {
  const factory _HomeScreenState({
    final List<SquareState> squares,
    final List<DeviceState> devices,
    required final Color backgroundFrom,
    required final Color backgroundTo,
    required final bool isDarkMode,
    required final double squareSizeScale,
    required final bool isEditMode,
    required final bool isErasingMode,
    required final bool isSharingMode,
    final String? selectedSquareId,
    final bool isLoading,
    final String? errorMessage,
  }) = _$HomeScreenStateImpl;

  @override
  List<SquareState> get squares; // Dashboardy (len metadáta – napr. pozície, poradie, zdieľanie)
  @override
  List<DeviceState> get devices;
  @override
  Color get backgroundFrom; // Farby pozadia
  @override
  Color get backgroundTo;
  @override
  bool get isDarkMode; // Mód
  @override
  double get squareSizeScale; // Globálna veľkosť dashboardov (1.0 = normálna veľkosť)
  @override
  bool get isEditMode; // Či je používateľ v editačnom móde
  @override
  bool get isErasingMode; // Či je používateľ v editačnom móde
  @override
  bool get isSharingMode; // Ak chce zdieľať
  @override
  String? get selectedSquareId; // ID vybraného dashboardu (ak je niečo vybrané)
  // Na budúcnosť
  @override
  bool get isLoading; // Loader
  @override
  String? get errorMessage;

  /// Create a copy of HomeScreenState
  /// with the given fields replaced by the non-null parameter values.
  @override
  @JsonKey(includeFromJson: false, includeToJson: false)
  _$$HomeScreenStateImplCopyWith<_$HomeScreenStateImpl> get copyWith =>
      throw _privateConstructorUsedError;
}
