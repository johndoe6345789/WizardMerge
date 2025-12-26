export default function Home() {
  return (
    <main className="min-h-screen p-8">
      <div className="max-w-6xl mx-auto">
        <h1 className="text-4xl font-bold mb-4">WizardMerge</h1>
        <p className="text-xl mb-8 text-gray-600">
          Intelligent Merge Conflict Resolution
        </p>
        
        <div className="grid gap-6 md:grid-cols-2">
          <div className="border rounded-lg p-6">
            <h2 className="text-2xl font-semibold mb-3">Three-Way Merge</h2>
            <p className="text-gray-600 mb-4">
              Advanced merge algorithm with dependency analysis at text and LLVM-IR levels
            </p>
            <ul className="list-disc list-inside space-y-2 text-sm">
              <li>28.85% reduction in conflict resolution time</li>
              <li>Merge suggestions for 70%+ of conflict blocks</li>
              <li>Smart auto-resolution patterns</li>
            </ul>
          </div>
          
          <div className="border rounded-lg p-6">
            <h2 className="text-2xl font-semibold mb-3">Visual Interface</h2>
            <p className="text-gray-600 mb-4">
              Clean, intuitive UI for reviewing and resolving conflicts
            </p>
            <ul className="list-disc list-inside space-y-2 text-sm">
              <li>Three-panel diff view</li>
              <li>Syntax highlighting</li>
              <li>Keyboard shortcuts</li>
            </ul>
          </div>
          
          <div className="border rounded-lg p-6">
            <h2 className="text-2xl font-semibold mb-3">Git Integration</h2>
            <p className="text-gray-600 mb-4">
              Seamless integration with Git workflows
            </p>
            <ul className="list-disc list-inside space-y-2 text-sm">
              <li>Detect and list conflicted files</li>
              <li>Mark files as resolved</li>
              <li>Command-line interface</li>
            </ul>
          </div>
          
          <div className="border rounded-lg p-6">
            <h2 className="text-2xl font-semibold mb-3">Smart Analysis</h2>
            <p className="text-gray-600 mb-4">
              Context-aware code understanding
            </p>
            <ul className="list-disc list-inside space-y-2 text-sm">
              <li>Semantic merge for JSON, YAML, XML</li>
              <li>Language-aware merging (AST-based)</li>
              <li>Auto-resolution suggestions</li>
            </ul>
          </div>
        </div>
        
        <div className="mt-8 p-6 bg-blue-50 rounded-lg">
          <h3 className="text-xl font-semibold mb-2">Getting Started</h3>
          <p className="text-gray-700">
            WizardMerge is currently in active development. See the{' '}
            <a href="https://github.com/johndoe6345789/WizardMerge" className="text-blue-600 underline">
              GitHub repository
            </a>{' '}
            for roadmap and progress.
          </p>
        </div>
      </div>
    </main>
  )
}
