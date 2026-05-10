import { useCallback, useEffect, useRef, useState } from "react";

export interface GamepadState {
  connected: boolean;
  name: string;
  leftStick: { x: number; y: number };
  rightStick: { x: number; y: number };
  buttons: boolean[];
  leftTrigger: number;
  rightTrigger: number;
}

const DEADZONE = 0.08;

function applyDeadzone(value: number): number {
  if (Math.abs(value) < DEADZONE) return 0;
  const sign = Math.sign(value);
  return sign * ((Math.abs(value) - DEADZONE) / (1 - DEADZONE));
}

const DEFAULT_STATE: GamepadState = {
  connected: false,
  name: "",
  leftStick: { x: 0, y: 0 },
  rightStick: { x: 0, y: 0 },
  buttons: [],
  leftTrigger: 0,
  rightTrigger: 0,
};

export function useGamepad(): GamepadState {
  const [state, setState] = useState<GamepadState>(DEFAULT_STATE);
  const rafRef = useRef<number>(0);

  const poll = useCallback(() => {
    const gamepads = navigator.getGamepads();
    const gp = gamepads[0];

    if (gp) {
      setState({
        connected: true,
        name: gp.id,
        leftStick: {
          x: applyDeadzone(gp.axes[0] ?? 0),
          y: applyDeadzone(gp.axes[1] ?? 0),
        },
        rightStick: {
          x: applyDeadzone(gp.axes[2] ?? 0),
          y: applyDeadzone(gp.axes[3] ?? 0),
        },
        buttons: gp.buttons.map((b) => b.pressed),
        leftTrigger: gp.buttons[6]?.value ?? 0,
        rightTrigger: gp.buttons[7]?.value ?? 0,
      });
    } else {
      setState((prev) => (prev.connected ? DEFAULT_STATE : prev));
    }

    rafRef.current = requestAnimationFrame(poll);
  }, []);

  useEffect(() => {
    rafRef.current = requestAnimationFrame(poll);
    return () => cancelAnimationFrame(rafRef.current);
  }, [poll]);

  return state;
}
