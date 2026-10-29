import { useEffect, useState, useRef } from 'react'

const API = 'http://127.0.0.1:8080'

const searchAlgorithms = [
  'Linear Search',
  'Sentinel Search',
  'Binary Search',
  'Fibonacci Search',
  'Indexed Sequential Search',
]

const sortAlgorithms = [
  'Bubble Sort',
  'Selection Sort',
  'Insertion Sort',
  'Merge Sort',
  'Quick Sort',
  'Heap Sort',
  'Counting Sort',
  'Radix Sort',
  'Bucket Sort',
]

function App() {
  const [query, setQuery] = useState('')
  const [searchAlgo, setSearchAlgo] = useState('Binary Search')
  const [sortAlgo, setSortAlgo] = useState('Quick Sort')
  const [results, setResults] = useState([])
  const [webResults, setWebResults] = useState([])
  const [isSearchingWeb, setIsSearchingWeb] = useState(false)
  const [includeWebSearch, setIncludeWebSearch] = useState(true)
  const [activeTab, setActiveTab] = useState('all') // 'all', 'local', 'web'
  const [stats, setStats] = useState(null)
  const [suggestions, setSuggestions] = useState([])
  const [selectedIndex, setSelectedIndex] = useState(-1)
  const [history, setHistory] = useState(() => JSON.parse(localStorage.getItem('searchHistory') || '[]'))
  const [error, setError] = useState('')
  const [webError, setWebError] = useState('')
  const [lastSearchedQuery, setLastSearchedQuery] = useState('')
  const inputRef = useRef(null)

  useEffect(() => {
    fetch(`${API}/api/stats`)
      .then(r => r.json())
      .then(setStats)
      .catch(() => setError('Backend is not running. Start SearchEngine.exe --server.'))
  }, [])

  // Autocomplete: triggers as soon as user types 'a' (length >= 1)
  useEffect(() => {
    const raw = query.trim()
    const prefix = raw.split(/\s+/).pop() || ''
    if (!prefix || prefix.length < 1) {
      setSuggestions([])
      setSelectedIndex(-1)
      return
    }
    const timer = setTimeout(() => {
      fetch(`${API}/api/autocomplete?prefix=${encodeURIComponent(prefix)}`)
        .then(r => r.json())
        .then(d => {
          setSuggestions(d.suggestions || [])
          setSelectedIndex(-1)
        })
        .catch(() => setSuggestions([]))
    }, 60)
    return () => clearTimeout(timer)
  }, [query])

  async function search(value = query) {
    const q = value.trim()
    if (!q) return
    setError('')
    setWebError('')
    setSuggestions([])
    setSelectedIndex(-1)
    setLastSearchedQuery(q)

    const nextHistory = [q, ...history.filter(x => x !== q)].slice(0, 8)
    setHistory(nextHistory)
    localStorage.setItem('searchHistory', JSON.stringify(nextHistory))

    // 1. Search local documents via C++ backend
    try {
      const res = await fetch(
        `${API}/api/search?q=${encodeURIComponent(q)}&search=${encodeURIComponent(searchAlgo)}&sort=${encodeURIComponent(sortAlgo)}`
      )
      const data = await res.json()
      if (!res.ok) throw new Error(data.error || 'Search failed')
      setResults(data.results || [])
      setStats(data.stats)
    } catch (e) {
      setError(e.message || 'Could not connect to backend search engine.')
    }

    // 2. Search web concurrently (via Wikipedia CORS API)
    if (includeWebSearch) {
      setIsSearchingWeb(true)
      try {
        const wikiUrl = `https://en.wikipedia.org/w/api.php?action=query&list=search&srsearch=${encodeURIComponent(q)}&utf8=&format=json&origin=*`
        const wikiRes = await fetch(wikiUrl)
        const wikiData = await wikiRes.json()

        const webItems = []
        if (wikiData.query && wikiData.query.search) {
          wikiData.query.search.forEach(item => {
            const cleanSnippet = item.snippet.replace(/<\/?[^>]+(>|$)/g, '')
            webItems.push({
              title: item.title,
              snippet: cleanSnippet,
              url: `https://en.wikipedia.org/?curid=${item.pageid}`,
              source: 'Wikipedia',
              wordcount: item.wordcount,
              timestamp: item.timestamp,
            })
          })
        }
        setWebResults(webItems)
      } catch (err) {
        console.warn('Web search fetch error:', err)
        setWebError('Web search could not reach external network.')
        setWebResults([])
      } finally {
        setIsSearchingWeb(false)
      }
    } else {
      setWebResults([])
      setIsSearchingWeb(false)
    }
  }

  function applySuggestion(s) {
    const parts = query.trim().split(/\s+/)
    parts[parts.length - 1] = s
    const next = parts.join(' ')
    setQuery(next)
    setSuggestions([])
    setSelectedIndex(-1)
    search(next)
  }

  async function rebuild() {
    try {
      await fetch(`${API}/api/index/rebuild`, { method: 'POST' })
      const s = await fetch(`${API}/api/stats`).then(r => r.json())
      setStats(s)
      setResults([])
    } catch {
      setError('Could not rebuild the index.')
    }
  }

  const currentPrefix = query.trim().split(/\s+/).pop()?.toLowerCase() || ''

  return (
    <div className="app">
      <header>
        <div>
          <div className="eyebrow">DSA &amp; WEB SEARCH ENGINE</div>
          <h1>Mini Search Engine</h1>
          <p>Local Document Indexing &bull; Trie Autocomplete &bull; TF-IDF &bull; Web Search</p>
        </div>
        <button className="secondary" onClick={rebuild} title="Reload files and re-index">
          Rebuild Local Index
        </button>
      </header>

      {error && <div className="error">{error}</div>}
      {webError && <div className="warning">{webError}</div>}

      <section className="searchPanel">
        <div className="searchBox">
          <input
            ref={inputRef}
            value={query}
            onChange={e => setQuery(e.target.value)}
            onKeyDown={e => {
              if (suggestions.length > 0) {
                if (e.key === 'ArrowDown') {
                  e.preventDefault()
                  setSelectedIndex(prev => (prev + 1) % suggestions.length)
                  return
                } else if (e.key === 'ArrowUp') {
                  e.preventDefault()
                  setSelectedIndex(prev => (prev - 1 + suggestions.length) % suggestions.length)
                  return
                } else if (e.key === 'Enter' && selectedIndex >= 0) {
                  e.preventDefault()
                  applySuggestion(suggestions[selectedIndex])
                  return
                } else if (e.key === 'Escape') {
                  setSuggestions([])
                  setSelectedIndex(-1)
                  return
                }
              }
              if (e.key === 'Enter') {
                search()
              }
            }}
            placeholder="Search algo, binary search, tree, dynamic..."
            autoFocus
          />
          <button onClick={() => search()}>Search</button>

          {suggestions.length > 0 && (
            <div className="suggestions">
              <div className="suggestionsHeader">
                <span>Suggestions ({suggestions.length})</span>
                <span className="hint">&uarr;&darr; navigate, Enter select</span>
              </div>
              {suggestions.map((s, idx) => {
                const startsWithPrefix = currentPrefix && s.toLowerCase().startsWith(currentPrefix)
                const prefixPart = startsWithPrefix ? s.slice(0, currentPrefix.length) : ''
                const restPart = startsWithPrefix ? s.slice(currentPrefix.length) : s

                return (
                  <button
                    key={s}
                    className={`suggestionItem ${idx === selectedIndex ? 'selected' : ''}`}
                    onClick={() => applySuggestion(s)}
                    onMouseEnter={() => setSelectedIndex(idx)}
                  >
                    <span className="sugIcon">&bull;</span>
                    <span className="sugText">
                      {startsWithPrefix ? (
                        <>
                          <strong className="matchedPrefix">{prefixPart}</strong>
                          {restPart}
                        </>
                      ) : (
                        s
                      )}
                    </span>
                  </button>
                )
              })}
            </div>
          )}
        </div>

        <div className="controlsRow">
          <div className="selectors">
            <label>
              Search Algorithm
              <select value={searchAlgo} onChange={e => setSearchAlgo(e.target.value)}>
                {searchAlgorithms.map(x => <option key={x}>{x}</option>)}
              </select>
            </label>
            <label>
              Ranking / Sorting
              <select value={sortAlgo} onChange={e => setSortAlgo(e.target.value)}>
                {sortAlgorithms.map(x => <option key={x}>{x}</option>)}
              </select>
            </label>
          </div>

          <div className="toggleOption">
            <label className="checkboxLabel">
              <input
                type="checkbox"
                checked={includeWebSearch}
                onChange={e => setIncludeWebSearch(e.target.checked)}
              />
              <span>Include Web Search</span>
            </label>
          </div>
        </div>
      </section>

      <section className="metrics">
        <Metric title="Local Documents" value={stats?.documents ?? '—'} />
        <Metric title="Unique Terms" value={stats?.uniqueTerms ?? '—'} />
        <Metric title="Comparisons" value={stats?.comparisons ?? '—'} />
        <Metric title="Search Time" value={stats ? `${Number(stats.milliseconds).toFixed(2)} ms` : '—'} />
        <Metric
          title="Web Results"
          value={includeWebSearch ? (isSearchingWeb ? 'Searching...' : `${webResults.length} found`) : 'Off'}
        />
      </section>

      {(results.length > 0 || webResults.length > 0 || lastSearchedQuery) && (
        <div className="tabBar">
          <button
            className={`tabBtn ${activeTab === 'all' ? 'active' : ''}`}
            onClick={() => setActiveTab('all')}
          >
            All Results ({results.length + webResults.length})
          </button>
          <button
            className={`tabBtn ${activeTab === 'local' ? 'active' : ''}`}
            onClick={() => setActiveTab('local')}
          >
            Local Documents ({results.length})
          </button>
          {includeWebSearch && (
            <button
              className={`tabBtn ${activeTab === 'web' ? 'active' : ''}`}
              onClick={() => setActiveTab('web')}
            >
              Web Results {isSearchingWeb ? '⏳' : `(${webResults.length})`}
            </button>
          )}
        </div>
      )}

      <main>
        <section>
          {results.length === 0 && webResults.length === 0 && !isSearchingWeb ? (
            <div className="empty">
              <strong>Start with a query.</strong>
              <p>Try &ldquo;algo&rdquo;, &ldquo;data structures&rdquo;, &ldquo;dynamic&rdquo;, or &ldquo;graph&rdquo;.</p>
              <div className="examplePills">
                {['algo', 'sort', 'search', 'tree', 'graph'].map(ex => (
                  <button key={ex} className="pill" onClick={() => { setQuery(ex); search(ex); }}>
                    {ex}
                  </button>
                ))}
              </div>
            </div>
          ) : (
            <>
              {/* LOCAL RESULTS */}
              {(activeTab === 'all' || activeTab === 'local') && (
                <div className="resultGroup">
                  <div className="groupHeader">
                    <h3>Local Documents ({results.length})</h3>
                    {results.length > 0 && <span className="algoBadge">{searchAlgo} &bull; {sortAlgo}</span>}
                  </div>

                  {results.length === 0 ? (
                    <div className="subEmpty">No local documents matched this query.</div>
                  ) : (
                    results.map((r, i) => (
                      <article className="result localResult" key={`local-${r.filename}-${i}`}>
                        <div className="resultTop">
                          <div>
                            <span className="rank">#{i + 1}</span>
                            <span className="sourceTag localTag">Local Document</span>
                            <h3>{r.filename}</h3>
                          </div>
                          <div className="scoreBadge" title="Cosine Similarity & TF-IDF match score">
                            <span className="scoreLabel">Match Score</span>
                            <span className="scoreValue">{Number(r.score).toFixed(3)}</span>
                          </div>
                        </div>
                        <p>{r.snippet}</p>
                        {r.matchingTerms && r.matchingTerms.length > 0 && (
                          <div className="terms">
                            <span className="termLabel">Matched:</span>
                            {r.matchingTerms.map(t => <span key={t} className="termBadge">{t}</span>)}
                          </div>
                        )}
                      </article>
                    ))
                  )}
                </div>
              )}

              {/* WEB RESULTS */}
              {includeWebSearch && (activeTab === 'all' || activeTab === 'web') && (
                <div className="resultGroup">
                  <div className="groupHeader">
                    <h3>Web Results ({webResults.length})</h3>
                    {isSearchingWeb && <span className="searchingIndicator">Searching Wikipedia...</span>}
                  </div>

                  {webResults.length === 0 && !isSearchingWeb ? (
                    <div className="subEmpty">No web results found.</div>
                  ) : (
                    webResults.map((wr, i) => (
                      <article className="result webResult" key={`web-${wr.url}-${i}`}>
                        <div className="resultTop">
                          <div>
                            <span className="sourceTag webTag">{wr.source}</span>
                            <h3>
                              <a href={wr.url} target="_blank" rel="noopener noreferrer" className="webLink">
                                {wr.title} <span className="externalIcon">&nearr;</span>
                              </a>
                            </h3>
                          </div>
                          {wr.wordcount && (
                            <span className="wordCountBadge">{wr.wordcount.toLocaleString()} words</span>
                          )}
                        </div>
                        <p>{wr.snippet}</p>
                        <div className="webFooter">
                          <a href={wr.url} target="_blank" rel="noopener noreferrer" className="visitLink">
                            Visit web page &rarr;
                          </a>
                        </div>
                      </article>
                    ))
                  )}
                </div>
              )}
            </>
          )}
        </section>

        <aside>
          <div className="sideCard">
            <h3>Recent Searches</h3>
            {history.length === 0 ? (
              <p className="muted">No searches yet.</p>
            ) : (
              history.map(h => (
                <button
                  className="history"
                  key={h}
                  onClick={() => {
                    setQuery(h)
                    search(h)
                  }}
                >
                  {h}
                </button>
              ))
            )}
          </div>

          {lastSearchedQuery && (
            <div className="sideCard">
              <h3>Web Portals</h3>
              <p className="portalDesc">Search &ldquo;{lastSearchedQuery}&rdquo; directly on:</p>
              <div className="portalLinks">
                <a
                  href={`https://www.google.com/search?q=${encodeURIComponent(lastSearchedQuery)}`}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="portalBtn"
                >
                  Google &nearr;
                </a>
                <a
                  href={`https://www.google.com/search?q=site%3Ageeksforgeeks.org+${encodeURIComponent(lastSearchedQuery)}`}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="portalBtn"
                >
                  GeeksforGeeks &nearr;
                </a>
                <a
                  href={`https://en.wikipedia.org/wiki/Special:Search?search=${encodeURIComponent(lastSearchedQuery)}`}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="portalBtn"
                >
                  Wikipedia &nearr;
                </a>
                <a
                  href={`https://stackoverflow.com/search?q=${encodeURIComponent(lastSearchedQuery)}`}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="portalBtn"
                >
                  Stack Overflow &nearr;
                </a>
                <a
                  href={`https://github.com/search?q=${encodeURIComponent(lastSearchedQuery)}`}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="portalBtn"
                >
                  GitHub &nearr;
                </a>
              </div>
            </div>
          )}

          <div className="sideCard">
            <h3>How Search Works</h3>
            <ol>
              <li><strong>Trie:</strong> Immediate prefix autocomplete on typing</li>
              <li><strong>Inverted Index:</strong> Locates exact &amp; potential matches</li>
              <li><strong>Search Algorithm:</strong> Locates candidate document IDs</li>
              <li><strong>TF-IDF &amp; Cosine:</strong> Computes document relevance</li>
              <li><strong>Sorting Algorithm:</strong> Orders documents by score</li>
              <li><strong>Web Engine:</strong> Fetches live online articles</li>
            </ol>
          </div>
        </aside>
      </main>
    </div>
  )
}

function Metric({ title, value }) {
  return (
    <div className="metric">
      <span>{title}</span>
      <strong>{value}</strong>
    </div>
  )
}

export default App
