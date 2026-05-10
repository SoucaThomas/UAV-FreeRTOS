import { useState, useEffect, useRef } from "react";
import type { GamepadState } from "./useGamepad";

export interface SimulatedFlightState {
  position: [number, number]; // [lng, lat]
  heading: number;            // degrees
  altitude: number;           // meters
  speed: number;              // m/s
  pitch: number;              // degrees
  roll: number;               // degrees
  distanceFromHome: number;   // meters
  trail: [number, number][];  // position history
  battery: number;            // percentage
}

const LNG_SCALE = 78000; // meters per degree longitude at ~46°
const LAT_SCALE = 111000; // meters per degree latitude

function distanceMeters(a: [number, number], b: [number, number]): number {
  const dx = (a[0] - b[0]) * LNG_SCALE;
  const dy = (a[1] - b[1]) * LAT_SCALE;
  return Math.sqrt(dx * dx + dy * dy);
}

function clamp(val: number, min: number, max: number): number {
  return Math.max(min, Math.min(max, val));
}

export function useSimulatedFlight(
  home: [number, number] | null,
  enabled: boolean,
  gamepad: GamepadState
): SimulatedFlightState | null {
  const [state, setState] = useState<SimulatedFlightState | null>(null);
  const simRef = useRef({
    lng: 0,
    lat: 0,
    heading: 0,
    altitude: 50,
    speed: 0,
    battery: 100,
    trail: [] as [number, number][],
  });
  const gamepadRef = useRef(gamepad);
  gamepadRef.current = gamepad;

  useEffect(() => {
    if (!home || !enabled) {
      setState(null);
      simRef.current = {
        lng: home?.[0] ?? 0,
        lat: home?.[1] ?? 0,
        heading: 0,
        altitude: 50,
        speed: 0,
        battery: 100,
        trail: [],
      };
      return;
    }

    // Initialize position at home
    simRef.current.lng = home[0];
    simRef.current.lat = home[1];

    const dt = 0.05; // 50ms tick

    const interval = setInterval(() => {
      const sim = simRef.current;
      const gp = gamepadRef.current;

      // --- Controls ---
      // Left stick Y (inverted): throttle → speed
      const throttleInput = clamp((-gp.leftStick.y + 1) / 2, 0, 1);
      // Right stick X: roll/turn
      const rollInput = gp.rightStick.x;
      // Right stick Y (inverted): pitch → climb/descend
      const pitchInput = -gp.rightStick.y;

      // --- Speed: throttle controls target speed, smooth acceleration ---
      const targetSpeed = throttleInput * 25; // max 25 m/s
      sim.speed += (targetSpeed - sim.speed) * 0.05; // smooth

      // --- Heading: roll input turns the plane ---
      const turnRate = rollInput * 120; // max 120 deg/s
      sim.heading += turnRate * dt;
      sim.heading = ((sim.heading % 360) + 360) % 360;

      // --- Roll: proportional to turn input ---
      const roll = rollInput * 45; // max 45 degrees bank

      // --- Altitude: pitch input climbs/descends ---
      const climbRate = pitchInput * 10; // max 10 m/s vertical
      sim.altitude += climbRate * dt;
      sim.altitude = clamp(sim.altitude, 5, 500);

      // --- Pitch: from climb rate ---
      const pitch = -pitchInput * 20; // visual pitch angle

      // --- Position: move in heading direction ---
      const headingRad = (sim.heading * Math.PI) / 180;
      const distancePerTick = sim.speed * dt; // meters
      sim.lng += (Math.sin(headingRad) * distancePerTick) / LNG_SCALE;
      sim.lat += (Math.cos(headingRad) * distancePerTick) / LAT_SCALE;

      // --- Battery: drains with throttle ---
      sim.battery -= throttleInput * 0.005;
      sim.battery = Math.max(0, sim.battery);

      // --- Trail ---
      const position: [number, number] = [sim.lng, sim.lat];
      if (sim.speed > 0.5) {
        sim.trail.push(position);
        if (sim.trail.length > 500) {
          sim.trail = sim.trail.slice(-500);
        }
      }

      setState({
        position,
        heading: sim.heading,
        altitude: sim.altitude,
        speed: sim.speed,
        pitch,
        roll,
        distanceFromHome: distanceMeters(position, home),
        trail: [...sim.trail],
        battery: sim.battery,
      });
    }, 50);

    return () => clearInterval(interval);
  }, [home, enabled]);

  return state;
}
