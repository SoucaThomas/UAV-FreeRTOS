import { useEffect, useMemo, useState } from "react";
import { useGamepad } from "@/hooks/useGamepad";
import { useSimulatedFlight } from "@/hooks/useSimulatedFlight";
import { AttitudeIndicator } from "@/components/AttitudeIndicator";
import { StickDisplay } from "@/components/StickDisplay";
import { ThrottleBar } from "@/components/ThrottleBar";
import { TelemetryPanel, type FlightState } from "@/components/TelemetryPanel";
import { MapView } from "@/components/MapView";
import { PacketLog } from "@/components/PacketLog";
import { FloatingPanel } from "@/components/FloatingPanel";
import { AirspaceStatus } from "@/components/AirspaceStatus";
import { useAirspace } from "@/hooks/useAirspace";
import { SettingsPanel, MAP_STYLES, type MapStyleName } from "@/components/SettingsPanel";
import { Badge } from "@/components/ui/badge";
import {
  Gamepad2,
  Activity,
  MessageSquare,
  Navigation,
  Radio,
  Plane,
  Shield,
  Settings,
} from "lucide-react";

type PanelId = "packets" | "telemetry" | "controls" | "settings";

export function App() {
  const gamepad = useGamepad();
  const [panels, setPanels] = useState<Record<PanelId, boolean>>({
    packets: true,
    telemetry: true,
    controls: true,
    settings: false,
  });
  const [simEnabled, setSimEnabled] = useState(false);
  const [showAirspace, setShowAirspace] = useState(false);
  const [mapStyle, setMapStyle] = useState<MapStyleName>("Dark Matter");
  const [airspaceOpacity, setAirspaceOpacity] = useState(0.35);
  const [homeLocation, setHomeLocation] = useState<[number, number] | null>(null);

  // Get home location for simulation
  useEffect(() => {
    if (!navigator.geolocation) {
      setHomeLocation([23.6, 46.77]);
      return;
    }
    navigator.geolocation.getCurrentPosition(
      (pos) => setHomeLocation([pos.coords.longitude, pos.coords.latitude]),
      () => setHomeLocation([23.6, 46.77]),
      { timeout: 5000 }
    );
  }, []);

  const flight = useSimulatedFlight(homeLocation, simEnabled, gamepad);
  const currentPosition = flight ? flight.position : homeLocation;
  const airspaces = useAirspace(showAirspace ? currentPosition : null);

  const toggle = (id: PanelId) =>
    setPanels((p) => ({ ...p, [id]: !p[id] }));

  // Telemetry: use simulated flight data when available, otherwise gamepad
  const telemetry = useMemo(() => {
    if (flight) {
      return {
        state: "FLIGHT" as FlightState,
        pitch: flight.pitch,
        roll: flight.roll,
        throttle: 0.6,
        batteryPercent: Math.round(flight.battery),
        batteryVoltage: 3.0 + (flight.battery / 100) * 1.2,
        radioConnected: true,
        distanceHome: flight.distanceFromHome,
        altitude: flight.altitude,
        speed: flight.speed,
      };
    }

    const pitch = gamepad.rightStick.y * -45;
    const roll = gamepad.rightStick.x * 45;
    const throttle = (-gamepad.leftStick.y + 1) / 2;

    let state: FlightState = "DISARMED";
    if (gamepad.connected) {
      if (throttle > 0.05) state = "FLIGHT";
      else if (gamepad.buttons[0]) state = "ARMED";
    }

    return {
      state,
      pitch,
      roll,
      throttle,
      batteryPercent: Math.max(20, Math.round(85 - throttle * 15)),
      batteryVoltage: 3.0 + (85 / 100) * 1.2,
      radioConnected: false,
      distanceHome: 0,
      altitude: 0,
      speed: 0,
    };
  }, [gamepad, flight]);

  const toggleButtons: { id: PanelId; icon: React.ReactNode; label: string }[] =
    [
      {
        id: "packets",
        icon: <MessageSquare className="h-4 w-4" />,
        label: "Packets",
      },
      {
        id: "telemetry",
        icon: <Activity className="h-4 w-4" />,
        label: "Telemetry",
      },
      {
        id: "controls",
        icon: <Navigation className="h-4 w-4" />,
        label: "Controls",
      },
      {
        id: "settings",
        icon: <Settings className="h-4 w-4" />,
        label: "Settings",
      },
    ];

  return (
    <div className="relative h-full w-full">
      {/* Full-screen map background */}
      <div className="absolute inset-0 z-0">
        <MapView flight={flight} showAirspace={showAirspace} airspaceOpacity={airspaceOpacity} mapStyle={MAP_STYLES[mapStyle]} />
      </div>

      {/* Top bar */}
      <header className="absolute top-0 left-0 right-0 z-20 flex h-10 items-center justify-between px-4 bg-[hsl(224,71.4%,4.1%)]/70 backdrop-blur-md border-b border-white/10">
        <div className="flex items-center gap-3">
          <Radio className="h-4 w-4 text-blue-400" />
          <h1 className="text-xs font-semibold tracking-widest text-[hsl(var(--foreground))] uppercase">
            UAV Ground Station
          </h1>
        </div>

        <div className="flex items-center gap-1">
          {/* Simulate flight toggle */}
          <button
            onClick={() => setSimEnabled(!simEnabled)}
            title="Simulate Flight"
            className={`flex items-center gap-1.5 rounded-md px-2 py-1 text-xs transition-colors ${
              simEnabled
                ? "bg-amber-500/20 text-amber-400"
                : "text-[hsl(var(--muted-foreground))] hover:bg-white/10 hover:text-[hsl(var(--foreground))]"
            }`}
          >
            <Plane className="h-4 w-4" />
            <span className="hidden sm:inline">Simulate</span>
          </button>

          {/* Airspace toggle */}
          <button
            onClick={() => setShowAirspace(!showAirspace)}
            title="Airspace Overlay"
            className={`flex items-center gap-1.5 rounded-md px-2 py-1 text-xs transition-colors ${
              showAirspace
                ? "bg-purple-500/20 text-purple-400"
                : "text-[hsl(var(--muted-foreground))] hover:bg-white/10 hover:text-[hsl(var(--foreground))]"
            }`}
          >
            <Shield className="h-4 w-4" />
            <span className="hidden sm:inline">Airspace</span>
          </button>

          <div className="mx-1 h-4 w-px bg-white/15" />

          {/* Panel toggle buttons */}
          {toggleButtons.map((btn) => (
            <button
              key={btn.id}
              onClick={() => toggle(btn.id)}
              title={btn.label}
              className={`flex items-center gap-1.5 rounded-md px-2 py-1 text-xs transition-colors ${
                panels[btn.id]
                  ? "bg-white/15 text-[hsl(var(--foreground))]"
                  : "text-[hsl(var(--muted-foreground))] hover:bg-white/10 hover:text-[hsl(var(--foreground))]"
              }`}
            >
              {btn.icon}
              <span className="hidden sm:inline">{btn.label}</span>
            </button>
          ))}

          <div className="mx-1 h-4 w-px bg-white/15" />

          {/* Controller status */}
          <div className="flex items-center gap-1.5">
            <Gamepad2 className="h-4 w-4 text-[hsl(var(--muted-foreground))]" />
            <Badge
              variant={gamepad.connected ? "success" : "secondary"}
              className="text-[10px] px-2 py-0"
            >
              {gamepad.connected ? "Connected" : "No Controller"}
            </Badge>
          </div>
        </div>
      </header>

      {/* Left panel - Packet Log */}
      <div className="absolute top-12 left-3 z-10 w-80">
        <FloatingPanel
          title="Packet Log"
          visible={panels.packets}
          onClose={() => toggle("packets")}
        >
          <PacketLog />
        </FloatingPanel>
      </div>

      {/* Right panel - Telemetry */}
      <div className="absolute top-12 right-3 z-10 w-64 max-h-[calc(100vh-180px)] overflow-hidden flex flex-col">
        <FloatingPanel
          title="Telemetry"
          visible={panels.telemetry}
          onClose={() => toggle("telemetry")}
        >
          <TelemetryPanel
            state={telemetry.state}
            batteryVoltage={telemetry.batteryVoltage}
            batteryPercent={telemetry.batteryPercent}
            radioConnected={telemetry.radioConnected}
            distanceHome={telemetry.distanceHome}
          />
          {/* Airspace status */}
          {showAirspace && (
            <div className="mt-3 border-t border-white/10 pt-3">
              <AirspaceStatus airspaces={airspaces} />
            </div>
          )}
          {/* Extra flight info when simulating */}
          {flight && (
            <div className="mt-3 space-y-1.5 border-t border-white/10 pt-3">
              <div className="flex items-center justify-between">
                <span className="text-[11px] text-[hsl(var(--muted-foreground))]">Altitude</span>
                <span className="text-[11px] font-mono text-[hsl(var(--foreground))]">{telemetry.altitude.toFixed(0)} m</span>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-[11px] text-[hsl(var(--muted-foreground))]">Speed</span>
                <span className="text-[11px] font-mono text-[hsl(var(--foreground))]">{telemetry.speed.toFixed(1)} m/s</span>
              </div>
            </div>
          )}
        </FloatingPanel>
      </div>

      {/* Settings panel */}
      <div className="absolute top-12 left-1/2 -translate-x-1/2 z-10 w-72">
        <FloatingPanel
          title="Settings"
          visible={panels.settings}
          onClose={() => toggle("settings")}
        >
          <SettingsPanel
            mapStyle={mapStyle}
            onMapStyleChange={setMapStyle}
            airspaceOpacity={airspaceOpacity}
            onAirspaceOpacityChange={setAirspaceOpacity}
          />
        </FloatingPanel>
      </div>

      {/* Bottom right - Attitude + Controls */}
      {panels.controls && (
        <div className="absolute bottom-4 right-3 z-10">
          <div className="flex flex-col items-center gap-2">
            {/* Attitude indicator */}
            <div className="rounded-xl border border-white/10 bg-[hsl(224,71.4%,4.1%)]/80 backdrop-blur-xl shadow-2xl p-2">
              <AttitudeIndicator
                pitch={telemetry.pitch}
                roll={telemetry.roll}
                size={110}
              />
            </div>
            {/* Sticks + Throttle */}
            <div className="flex items-center gap-2 rounded-xl border border-white/10 bg-[hsl(224,71.4%,4.1%)]/80 backdrop-blur-xl shadow-2xl p-2">
              <StickDisplay
                label="L"
                x={gamepad.leftStick.x}
                y={gamepad.leftStick.y}
                size={70}
              />
              <ThrottleBar value={telemetry.throttle} />
              <StickDisplay
                label="R"
                x={gamepad.rightStick.x}
                y={gamepad.rightStick.y}
                size={70}
              />
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
