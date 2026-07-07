I developed this three-level cache simulator as part of the "Grundlagenpraktikum Rechnerarchitektur" (Computer Architecture Lab) at TUM. The project implements a cycle-based SystemC/C++ simulation of a three-layer cache hierarchy, including multiple replacement and mapping strategies. The following is the original German Readme which describes the implementation and results in more detail.

# Cache-Simulation mit SystemC

In den letzten Jahrzehnten stieg die Geschwindigkeit von CPUs deutlich schneller als die von RAM, wodurch Prozesse lange auf Daten warten müssen (Von-Neumann-Flaschenhals). Caches beschleunigen den Zugriff, indem sie häufig genutzte Daten in einem kleinen, schnellen Zwischenspeicher bereithalten.

Moderne Caches bestehen aus mehreren **Cache-Levels** (L1, L2, L3). Niedrigere Ebenen sind größer aber auch langsamer.


Caches nutzen räumliche und zeitliche Lokalität.
Räumliche Lokalität bedeutet, dass nahe beieinanderliegende Daten gemeinsam genutzt werden, daher werden Speicherbereiche in **Cachezeilen** (z. B. 64 Byte) geladen. Zeitliche Lokalität besagt, dass kürzlich verwendete Daten bald wieder genutzt werden, weshalb ungenutzte Cachezeilen verdrängt werden (abhängig von der Replacement Strategie).

## Mapping-Strategien

- **Direct-Mapped:** Jede Cachezeile hat einen festen Platz, einfache Implementierung, hohe Kollisionswahrscheinlichkeit.
- **Fully Associative:** Jede Zeile kann überall gespeichert werden, reduziert Kollisionen, aber aufwendig.
- **Set-Associative:** Kompromiss, der Cache wird in Sets organisiert, innerhalb eines Sets freie Platzwahl.

Beim Cache-Zugriff wird die Speicheradresse in Tag, Index und Offset unterteilt. Der Tag identifiziert die Daten, der Index bestimmt die Cachezeile oder das Set, und der Offset gibt die genaue Position innerhalb einer Cachezeile an.

In einem direct-mapped Cache gibt der Index die feste Speicherposition vor, der Tag prüft, ob die Daten bereits dort liegen. Ein fully associative Cache erlaubt beliebige Speicherorte, braucht also keinen Index.

Funktionsweise eines set-associative Caches:
![Cache](img/img.png)

## Intel Core i7-12700K

Zur besseren Einschätzung typischer Kenndaten betrachten wir den Intel Core i7-12700K (Alder Lake, 12. Generation), der häufig in Gaming-PCs und Workstations verwendet wird.

| **Cache-Level** | **Größe** | **Assoziativität** | **Cache-Latenz** |
|---------------|-------|----------------|--------------|
| **L1D**      | 48 KB | 12-Wege        | ~4 Zyklen    |
| **L1I**      | 32 KB | 8-Wege         | ~4 Zyklen    |
| **L2**       | 1.25 MB | 10-Wege        | ~12 Zyklen   |
| **L3 (Shared)** | 25 MB | 20-Wege        | ~36–40 Zyklen |

Jeder Kern hat eigene L1 und L2 Caches. Der L3 Cache wird geteilt.

Es gibt zwei L1 Caches, einen für Daten und einen für Instruktionen (Harvard Architektur).
In allen Levels sind die Cache Lines 64 Bytes groß.

Ein RAM-Zugriff (DDR5) braucht zum Vergleich etwa 350 Zyklen.

## Implementierung

Ein in **SystemC** simulierter Cache mit bis zu drei Ebenen (L1, L2, L3) untersucht die drei Mapping-Strategien. Der Cache ist als **CACHE**-Modul realisiert und über Ports mit dem **MAIN_MEMORY**-Modul verbunden. Die drei Ebenen sind Instanzen desselben SystemC-Moduls und nutzen eine einheitliche Implementierung. Aus Gründen der Übersichtlichkeit, sind Cache Sets und Cachezeilen durch C++ Klassen implementiert.

Die Mapping-Strategien erfordern keine spezielle Fallunterscheidung, da direct-mapped und fully associative lediglich Sonderfälle des set-associative Caches sind: Ein direct-mapped Cache entspricht einem set-associative Cache mit nur einer Cachezeile pro Set, während ein fully associative Cache nur ein einziges Set besitzt.

Die verwendeten Datenstrukturen ermöglichen read- und write-Zugriffe in amortisiert konstanter Zeit (unabhängig von der Cache-Größe): **Cache-Sets** werden über eine **unordered map** adressiert, während **Cachezeilen** in einer **doppelt verketteten Liste** gespeichert sind, um **Least Recently Used (LRU)** effizient umzusetzen. Eine weitere unordered map ermöglicht den direkten Zugriff auf die Cachezeilen durch das Mapping von Tags auf Listen-Iteratoren.

Die Tests überprüfen das Verhalten bei Cache-Misses, Ersetzungen, Mehr-Level-Nutzung und Zugriffen über mehrere Cachezeilen.

Aufgrund unseres Nachteilsausgleichs muss die Anzahl der Gatter nicht abgeschätzt werden.

## Replacement-Strategien

- **Least Recently Used (LRU):** Verdrängt die am längsten nicht genutzte Zeile, aufwendig, aber lokalitätsfreundlich.
- **First-In First-Out (FIFO):** Einfach, aber verdrängt auch kürzlich genutzte Daten.
- **Least Frequently Used (LFU):** Nutzt Zugriffszähler, gut für stabile Relevanz (z. B. Datenbanken).
- **Random Replacement:** Zufällige Ersetzung, einfach, aber ineffizient bei Lokalität. Wird teilweise in L3-Caches verwendet.

## Speicherzugriffsverhalten Matrixmultiplikation

Die Effizienz wurde anhand einer naiven **32×32-Integer-Matrixmultiplikation** gemessen. Dabei wurden zwei Matrizen $A$ und $B$ multipliziert und das Ergebnis in $C$ gespeichert. Die Implementierung hat eine kubische Laufzeit, wobei für jedes Element von $C$ ein Skalarprodukt berechnet wird. Um realistischere Speicherzugriffe zu simulieren, wurde angenommen, dass das Zwischenergebnis des Skalarprodukts in einem CPU-Register gespeichert wird, wodurch die Anzahl der Schreibzugriffe um den Faktor 32 reduziert wird.

Untersucht wurde der Einfluss der Cache-Level und der Mapping-Strategie auf Hits und Misses. Besonders auffällig war der langsamere Zugriff auf $B$, da die Werte spaltenweise gelesen werden und im Hauptspeicher nicht nebeneinander liegen. Eine mögliche Optimierung wäre die Transposition von $B$ vor der Multiplikation.

32 Byte Cachezeilen, L1: 32 Zeilen (4 Zyklen), L2: 128 Zeilen (12 Zyklen), L3: 512 Zeilen (36 Zyklen)

| **Verfahren**                   | **Hits** | **Misses** | **Cycles** |
|---------------------------------|-------|-----------|-----------|
| **direct-mapped (1 Level)**     |    30656   |   35904        | 31M       |
| **fully associative (1 Level)** |    32640   |     33920      | 30M       |
| **set-associative (1 Level)**   |    31552   |     35008      | 31M       |
| **set-associative (2 Level)**   |    55896   |      10664     | 11M       |
| **set-associative (3 Level)**   |   66176    |     384      | 2M        |

Wie erwartet schneiden set-associative Caches besser ab als direct-mapped, aber schlechter als fully associative Caches. Bei 3 Levels wurde die gesamte Matrix in den Cache geladen, sodass nur beim ersten Zugriff Misses auftraten.

## Quellen

- [Intel Core i7-12700K – Technische Daten](https://www.intel.de/content/www/de/de/products/sku/134594/intel-core-i712700k-processor-25m-cache-up-to-5-00-ghz/specifications.html)
- [Replacement Strategies](https://hazelcast.com/foundations/caching/caching-strategies/?utm_source=chatgpt.com)
- [Multi Level Caches](https://library.fiveable.me/advanced-computer-architecture/unit-8/multi-level-cache-hierarchies/study-guide/DFgwyEGiVkXjgHEe)
- [GeeksforGeeks – Cache Memory](https://www.geeksforgeeks.org/cache-memory-in-computer-organization/)