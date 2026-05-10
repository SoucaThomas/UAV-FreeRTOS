import { useMemo } from "react";
import { ArrowUpRight, ArrowDownLeft } from "lucide-react";

interface Packet {
  id: number;
  direction: "TX" | "RX";
  type: string;
  timestamp: string;
  payload: string;
}

function generateMockPackets(): Packet[] {
  const types = [
    { type: "HEARTBEAT", payload: "seq=142 state=DISARMED" },
    { type: "GPS_POS", payload: "lat=46.770 lon=23.600 alt=340m" },
    { type: "ATTITUDE", payload: "p=2.1 r=-0.8 y=45.2" },
    { type: "BATTERY", payload: "v=11.82 pct=84 mah=620" },
    { type: "RC_INPUT", payload: "ch1=1500 ch2=1500 ch3=1000 ch4=1500" },
    { type: "RADIO_STATUS", payload: "rssi=-62 snr=12 txpwr=20" },
    { type: "CMD_ACK", payload: "cmd=ARM result=ACCEPTED" },
    { type: "PARAM_VALUE", payload: "PID_P=0.45 idx=3/24" },
    { type: "SYS_STATUS", payload: "cpu=23% mem=41% temp=38C" },
    { type: "WAYPOINT", payload: "wp=3/8 dist=142m eta=28s" },
  ];

  const now = new Date();
  return types.map((t, i) => ({
    id: i,
    direction: i % 3 === 0 ? "TX" : "RX",
    type: t.type,
    timestamp: new Date(now.getTime() - (types.length - i) * 1200)
      .toLocaleTimeString("en-GB", { hour12: false })
      .slice(0, 8) + "." + String(Math.floor(Math.random() * 999)).padStart(3, "0"),
    payload: t.payload,
  }));
}

export function PacketLog() {
  const packets = useMemo(() => generateMockPackets(), []);

  return (
    <div className="flex flex-col gap-1 max-h-[60vh] overflow-y-auto pr-1 custom-scrollbar">
      {packets.map((pkt) => (
        <div
          key={pkt.id}
          className="flex items-start gap-2 rounded-md bg-white/5 px-2 py-1.5 text-xs font-mono"
        >
          <span className="mt-0.5 shrink-0">
            {pkt.direction === "TX" ? (
              <ArrowUpRight className="h-3 w-3 text-blue-400" />
            ) : (
              <ArrowDownLeft className="h-3 w-3 text-emerald-400" />
            )}
          </span>
          <div className="min-w-0 flex-1">
            <div className="flex items-center gap-2">
              <span
                className={
                  pkt.direction === "TX"
                    ? "text-blue-400 font-semibold"
                    : "text-emerald-400 font-semibold"
                }
              >
                {pkt.direction}
              </span>
              <span className="text-[hsl(var(--foreground))] font-semibold">
                {pkt.type}
              </span>
              <span className="text-[hsl(var(--muted-foreground))] ml-auto tabular-nums">
                {pkt.timestamp}
              </span>
            </div>
            <p className="text-[hsl(var(--muted-foreground))] truncate mt-0.5">
              {pkt.payload}
            </p>
          </div>
        </div>
      ))}
    </div>
  );
}
