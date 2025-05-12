void print_time_stats(vector<ll>& time, const string& label) {
    if (time.empty()) {
        cout << "No timing data available for " << label << ".\n";
        return;
    }

    ll total = accumulate(time.begin(), time.end(), 0LL);
    ll avg = total / time.size();
    auto [min_t, max_t] = minmax_element(time.begin(), time.end());

    cout << "Time at " << label << ":\n";
    cout << "  Total: " << total << " ms\n";
    cout << "  Average: " << avg << " ms\n";
    cout << "  Min: " << *min_t << " ms\n";
    cout << "  Max: " << *max_t << " ms\n";

    time.clear();
}

void print_time_now(ll time) {
    // Optional debug print
    // cout << "Current Time: " << time << " ms\n";
}

void print_duration(chrono::nanoseconds ns, const string& label) {
    if (ns.count() == 0) {
        cout << "No timing data available for " << label << ".\n";
        return;
    }
    cout << "Time at " << label << ": " << fixed << setprecision(2) << ns.count() / 1e9 << " s\n";
}

