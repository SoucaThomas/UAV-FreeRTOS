import { X } from "lucide-react";

interface FloatingPanelProps {
  title: string;
  visible: boolean;
  onClose: () => void;
  children: React.ReactNode;
  className?: string;
}

export function FloatingPanel({
  title,
  visible,
  onClose,
  children,
  className = "",
}: FloatingPanelProps) {
  if (!visible) return null;

  return (
    <div
      className={`rounded-xl border border-white/10 bg-[hsl(224,71.4%,4.1%)]/80 backdrop-blur-xl shadow-2xl ${className}`}
    >
      <div className="flex items-center justify-between border-b border-white/10 px-3 py-2">
        <h2 className="text-xs font-semibold tracking-wide text-[hsl(var(--foreground))] uppercase">
          {title}
        </h2>
        <button
          onClick={onClose}
          className="rounded p-0.5 text-[hsl(var(--muted-foreground))] hover:bg-white/10 hover:text-[hsl(var(--foreground))] transition-colors"
        >
          <X className="h-3.5 w-3.5" />
        </button>
      </div>
      <div className="p-3 overflow-y-auto max-h-[inherit]">{children}</div>
    </div>
  );
}
