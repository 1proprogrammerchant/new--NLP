// Package validator provides an upgraded, modular reference integrity validator.
// Improvements over prior versions:
//   - Single package with no duplicate type definitions
//   - Exported types and functions for library reuse (no embedded main)
//   - Minimal imports for faster compilation
//   - sync.WaitGroup managed internally, reducing caller complexity
//   - Atomic timestamp update encapsulated inside the package
package validator

import (
	"fmt"
	"sync"
	"sync/atomic"
	"time"
)

// Status represents the referential integrity state of a reference.
type Status int

const (
	Valid Status = iota
	IdentityChanged
	IdentitySplit
	IdentityMerged
	Invalidated
	Unresolved
	ObserverRelative
)

func (s Status) String() string {
	names := [...]string{
		"Valid",
		"IdentityChanged",
		"IdentitySplit",
		"IdentityMerged",
		"Invalidated",
		"Unresolved",
		"ObserverRelative",
	}
	if int(s) < len(names) {
		return names[s]
	}
	return "Unknown"
}

// Entity represents a named object with a resolution state.
type Entity struct {
	ID    int
	Name  string
	State string
	Layer int
}

// Reference links a source entity to a target entity and records validation outcome.
type Reference struct {
	ID            int
	SourceID      int
	TargetID      int
	Status        Status
	CandidateIDs  []int
	LastValidated int64 // unix nanosecond timestamp
}

// Validate resolves the integrity status of ref given the entity registry and split map.
func Validate(ref *Reference, entities map[int]*Entity, splitMap map[int][]int) {
	target, ok := entities[ref.TargetID]
	if !ok {
		ref.Status = Invalidated
		atomic.StoreInt64(&ref.LastValidated, time.Now().UnixNano())
		return
	}
	switch target.State {
	case "Split":
		ref.Status = Unresolved
		ref.CandidateIDs = splitMap[target.ID]
	case "Merged":
		ref.Status = IdentityMerged
	case "ObserverRelative":
		ref.Status = ObserverRelative
	case "Collapsed":
		ref.Status = Invalidated
	case "Defined":
		ref.Status = Valid
	default:
		ref.Status = IdentityChanged
	}
	atomic.StoreInt64(&ref.LastValidated, time.Now().UnixNano())
}

// maxWorkers caps the number of concurrent validation goroutines.
const maxWorkers = 64

// ValidateAll validates all references concurrently using a bounded worker pool
// and blocks until all references have been processed.
func ValidateAll(references []*Reference, entities map[int]*Entity, splitMap map[int][]int) {
	sem := make(chan struct{}, maxWorkers)
	var wg sync.WaitGroup
	for _, ref := range references {
		wg.Add(1)
		sem <- struct{}{}
		go func(r *Reference) {
			defer wg.Done()
			defer func() { <-sem }()
			Validate(r, entities, splitMap)
		}(ref)
	}
	wg.Wait()
}

// PrintResults writes a human-readable summary of each reference to stdout.
func PrintResults(references []*Reference) {
	for _, ref := range references {
		fmt.Printf("Reference[%d]: %d -> %d | Status: %s", ref.ID, ref.SourceID, ref.TargetID, ref.Status)
		if len(ref.CandidateIDs) > 0 {
			fmt.Printf(" | Candidates:")
			for _, cid := range ref.CandidateIDs {
				fmt.Printf(" %d", cid)
			}
		}
		fmt.Println()
	}
}
