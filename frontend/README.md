# WizardMerge Frontend

Next.js-based web frontend for WizardMerge.

## Runtime

- **Package Manager**: bun
- **Framework**: Next.js 14
- **Language**: TypeScript
- **Styling**: Tailwind CSS (planned)

## Setup

### Prerequisites

```sh
# Install bun
curl -fsSL https://bun.sh/install | bash
```

### Development

```sh
# Install dependencies
bun install

# Run development server
bun run dev

# Build for production
bun run build

# Start production server
bun run start
```

The application will be available at http://localhost:3000

## Project Structure

```
frontend/
├── app/                 # Next.js app directory
│   ├── layout.tsx      # Root layout
│   ├── page.tsx        # Home page
│   └── globals.css     # Global styles
├── public/             # Static assets
├── package.json        # Dependencies
├── tsconfig.json       # TypeScript config
└── next.config.js      # Next.js config
```

## Features (Planned)

- Three-panel diff viewer
- Conflict resolution interface
- Real-time collaboration
- Syntax highlighting
- Integration with C++ backend via REST API

## Scripts

- `bun run dev` - Start development server
- `bun run build` - Build for production
- `bun run start` - Start production server
- `bun run lint` - Run ESLint
